#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "html.h"
#include "parser.h"
#include "string_buffer.h"
#include "util.h"
#include "vector.h"

static const size_t kMessageBufferSize = 256;

// Prints a formatted message to a StringBuffer.  This automatically resizes the
// StringBuffer as necessary to fit the message.  Returns the number of bytes
// written.
static int print_message(Parser* parser, StringBuffer* output, const char* format, ...) {
  va_list args;
  va_start(args, format);
  int remaining_capacity = output->capacity - output->length;
  int bytes_written = vsnprintf(output->data + output->length, remaining_capacity, format, args);
  if (bytes_written > remaining_capacity) {
    string_buffer_reserve(parser, output->capacity + bytes_written, output);
    remaining_capacity = output->capacity - output->length;
    bytes_written = vsnprintf(output->data + output->length, remaining_capacity, format, args);
  }
  output->length += bytes_written;
  va_end(args);
  return bytes_written;
}

static void print_tag_stack(Parser* parser, const ParserError* error, StringBuffer* output) {
  print_message(parser, output, "  Currently open tags: ");
  for (int i = 0; i < error->tag_stack.length; ++i) {
    if (i) {
      print_message(parser, output, ", ");
    }
    HTMLTag tag = (HTMLTag) error->tag_stack.data[i];
    print_message(parser, output, normalized_tagname(tag));
  }
  string_buffer_append_codepoint(parser, '.', output);
}

static void handle_parser_error(Parser* parser, const ParserError* error, StringBuffer* output) {
  if (error->parser_state == INSERTION_MODE_INITIAL && error->input_type != TOKEN_DOCTYPE) {
    print_message(parser, output, "The doctype must be the first token in the document");
    return;
  }

  switch (error->input_type) {
    case TOKEN_DOCTYPE:
      print_message(parser, output, "This is not a legal doctype");
      return;
    case TOKEN_COMMENT:
      // Should never happen; comments are always legal.
      assert(0);
      // But just in case...
      print_message(parser, output, "Comments aren't legal here");
      return;
    case TOKEN_WHITESPACE:
    case TOKEN_CHARACTER:
      print_message(parser, output, "Character tokens aren't legal here");
      return;
    case TOKEN_NULL:
      print_message(parser, output, "Null bytes are not allowed in HTML5");
      return;
    case TOKEN_EOF:
      if (error->parser_state == INSERTION_MODE_INITIAL) {
        print_message(parser, output, "You must provide a doctype");
      } else {
        print_message(parser, output, "Premature end of file");
        print_tag_stack(parser, error, output);
      }
      return;
    case TOKEN_START_TAG:
    case TOKEN_END_TAG:
      print_message(parser, output, "That tag isn't allowed here");
      print_tag_stack(parser, error, output);
      // TODO(jdtang): Give more specific messaging.
      return;
  }
}

// Finds the preceding newline in an original source buffer from a given byte
// location.  Returns a character pointer to the character after that, or a
// pointer to the beginning of the string if this is the first line.
static const char* find_last_newline(const char* original_text, const char* error_location) {
  assert(error_location >= original_text);
  const char* c = error_location;
  for (; c != original_text && *c != '\n'; --c) {
    // There may be an error at EOF, which would be a nul byte.
    assert(*c || c == error_location);
  }
  return c == original_text ? c : c + 1;
}

// Finds the next newline in the original source buffer from a given byte
// location.  Returns a character pointer to that newline, or a pointer to the
// terminating null byte if this is the last line.
static const char* find_next_newline(const char* original_text, const char* error_location) {
  const char* c = error_location;
  for (; *c && *c != '\n'; ++c);
  return c;
}

Error* add_error(Parser* parser) {
  int max_errors = parser->_options->max_errors;
  if (max_errors < 0 && parser->_output->errors.length >= max_errors) {
    return NULL;
  }
  Error* error = parser_allocate(parser, sizeof(Error));
  vector_add(parser, error, &parser->_output->errors);
  return error;
}

void error_to_string(Parser* parser, const Error* error, StringBuffer* output) {
  print_message(parser, output, "@%d:%d: ", error->position.line, error->position.column);
  switch (error->type) {
    case ERR_UTF8_INVALID:
      print_message(parser, output, "Invalid UTF8 character 0x%x", error->v.codepoint);
      break;
    case ERR_UTF8_TRUNCATED:
      print_message(parser, output, "Input stream ends with a truncated UTF8 character 0x%x", error->v.codepoint);
      break;
    case ERR_NUMERIC_CHAR_REF_NO_DIGITS:
      print_message(parser, output, "No digits after &# in numeric character reference");
      break;
    case ERR_NUMERIC_CHAR_REF_WITHOUT_SEMICOLON:
      print_message(parser, output, "The numeric character reference &#%d should be followed "
                    "by a semicolon", error->v.codepoint);
      break;
    case ERR_NUMERIC_CHAR_REF_INVALID:
      print_message(parser, output, "The numeric character reference &#%d; encodes an invalid "
               "unicode codepoint", error->v.codepoint);
      break;
    case ERR_NAMED_CHAR_REF_WITHOUT_SEMICOLON:
      // The textual data came from one of the literal strings in the table, and
      // so it'll be null-terminated.
      print_message(parser, output, "The named character reference &%.*s should be followed by a "
               "semicolon", (int) error->v.text.length, error->v.text.data);
      break;
    case ERR_NAMED_CHAR_REF_INVALID:
      print_message(parser, output, "The named character reference &%.*s; is not a valid entity name", (int) error->v.text.length, error->v.text.data);
      break;
    case ERR_DUPLICATE_ATTR:
      print_message(parser, output, "Attribute %s occurs multiple times, at positions %d and %d", error->v.duplicate_attr.name, error->v.duplicate_attr.original_index, error->v.duplicate_attr.new_index);
      break;
    case ERR_PARSER:
    case ERR_UNACKNOWLEDGED_SELF_CLOSING_TAG:
      handle_parser_error(parser, &error->v.parser, output);
      break;
    default:
      print_message(parser, output, "Tokenizer error with an unimplemented error message");
      break;
  }
  string_buffer_append_codepoint(parser, '.', output);
}

void caret_diagnostic_to_string(Parser* parser, const Error* error, const char* source_text, StringBuffer* output) {
  error_to_string(parser, error, output);

  const char* line_start = find_last_newline(source_text, error->original_text);
  const char* line_end = find_next_newline(source_text, error->original_text);
  StringPiece original_line;
  original_line.data = line_start;
  original_line.length = line_end - line_start;

  string_buffer_append_codepoint(parser, '\n', output);
  string_buffer_append_string(parser, &original_line, output);
  string_buffer_append_codepoint(parser, '\n', output);
  string_buffer_reserve(parser, output->length + error->position.column, output);
  int num_spaces = error->position.column - 1;
  memset(output->data + output->length, ' ', num_spaces);
  output->length += num_spaces;
  string_buffer_append_codepoint(parser, '^', output);
  string_buffer_append_codepoint(parser, '\n', output);
}

void print_caret_diagnostic(Parser* parser, const Error* error, const char* source_text) {
  StringBuffer text;
  string_buffer_init(parser, &text);
  caret_diagnostic_to_string(parser, error, source_text, &text);
  printf("%.*s", (int) text.length, text.data);
  string_buffer_destroy(parser, &text);
}

void error_destroy(Parser* parser, Error* error) {
  if (error->type == ERR_PARSER || error->type == ERR_UNACKNOWLEDGED_SELF_CLOSING_TAG) {
    vector_destroy(parser, &error->v.parser.tag_stack);
  } else if (error->type == ERR_DUPLICATE_ATTR) {
    parser_deallocate(parser, (void*) error->v.duplicate_attr.name);
  }
  parser_deallocate(parser, error);
}

void init_errors(Parser* parser) {
  vector_init(parser, 5, &parser->_output->errors);
}

void destroy_errors(Parser* parser) {
  for (int i = 0; i < parser->_output->errors.length; ++i) {
    error_destroy(parser, parser->_output->errors.data[i]);
  }
  vector_destroy(parser, &parser->_output->errors);
}
