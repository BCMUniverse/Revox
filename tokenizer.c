/*
	Este arquivo faz parte do BCM Revox Engine;

	BCM Revox Engine é Software Livre; você pode redistribui-lo e/ou
	modificá-lo dentro dos termos da Licença Pública Geru GNU como
	publicada pela Fundação do Software Livre (FSF); na versão 3 da Licença.
	Este programa é distribuído na esperança que possa ser util,
	mas SEM NENHUMA GARANTIA; sem uma garantia implicita de ADEQUAÇÂO a
	qualquer MERCADO ou APLICAÇÃO EM PARTICULAR. Veja a Licença Pública Geral
	GNU para maiores detalhes.
	Você deve ter recebido uma cópia da Licença Pública Geral GNU junto com
	este programa, se não, escreva para a Fundação do Software Livre(FSF) Inc.,
	51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

	BCM Revox Engine v0.1
	BCM Revox Engine -> Ano: 2013|Tipo: WebEngine
*/
// Copyright 2010 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: jdtang@google.com (Jonathan Tang)
//
// We use Gumbo as a prefix for types, gumbo_ as a prefix for functions, and
// GUMBO_ as a prefix for enum constants (static constants get the Google-style
// kGumbo prefix).
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "tokenizer.h"
#include "attribute.h"
#include "char_ref.h"
#include "error.h"
#include "html.h"
#include "parser.h"
#include "string_buffer.h"
#include "string_piece.h"
#include "token_type.h"
#include "tokenizer_states.h"
#include "utf8.h"
#include "util.h"
#include "vector.h"

const StringPiece kScriptTag = { "script", 6 };

typedef enum {
  RETURN_ERROR,
  RETURN_SUCCESS,
  NEXT_CHAR
} StateResult;

typedef struct InternalTagState {
  StringBuffer _buffer;
  const char* _original_text;
  HTMLTag _tag;
  SrcPosition _start_pos;
  Vector /* Attribute */ _attributes;
  bool _drop_next_attr_value;
  TokenizerEnum _attr_value_state;
  HTMLTag _last_start_tag;
  bool _is_start_tag;
  bool _is_self_closing;
} TagState;

typedef struct InternalTokenizerState {
  TokenizerEnum _state;
  bool _reconsume_current_input;
  bool _is_current_node_foreign;
  int _buffered_emit_char;
  StringBuffer _temporary_buffer;
  const char* _temporary_buffer_emit;
  StringBuffer _script_data_buffer;
  const char* _token_start;
  SrcPosition _token_start_pos;
  TagState _tag_state;
  TokenDocType _doc_type_state;
  Utf8Iterator _input;
}TokenizerState;
static void add_parse_error(Parser* parser, ErrorType type) {
  Error* error = add_error(parser);
  if (!error) {
    return;
  }
  TokenizerState* tokenizer = parser->_tokenizer_state;
  utf8iterator_get_position(&tokenizer->_input, &error->position);
  error->original_text = utf8iterator_get_char_pointer(&tokenizer->_input);
  error->type = type;
  error->v.tokenizer.codepoint = utf8iterator_current(&tokenizer->_input);
  switch (tokenizer->_state) {
    case DATA:
      error->v.tokenizer.state = ERR_TOKENIZER_DATA;
      break;
    case CHAR_REF_IN_DATA:
    case CHAR_REF_IN_RCDATA:
    case CHAR_REF_IN_ATTR_VALUE:
      error->v.tokenizer.state = ERR_TOKENIZER_CHAR_REF;
      break;
    case RCDATA:
    case RCDATA_LT:
    case RCDATA_END_TAG_OPEN:
    case RCDATA_END_TAG_NAME:
      error->v.tokenizer.state = ERR_TOKENIZER_RCDATA;
      break;
    case RAWTEXT:
    case RAWTEXT_LT:
    case RAWTEXT_END_TAG_OPEN:
    case RAWTEXT_END_TAG_NAME:
      error->v.tokenizer.state = ERR_TOKENIZER_RAWTEXT;
      break;
    case PLAINTEXT:
      error->v.tokenizer.state = ERR_TOKENIZER_PLAINTEXT;
      break;
    case SCRIPT:
    case SCRIPT_LT:
    case SCRIPT_END_TAG_OPEN:
    case SCRIPT_END_TAG_NAME:
    case SCRIPT_ESCAPED_START:
    case SCRIPT_ESCAPED_START_DASH:
    case SCRIPT_ESCAPED:
    case SCRIPT_ESCAPED_DASH:
    case SCRIPT_ESCAPED_DASH_DASH:
    case SCRIPT_ESCAPED_LT:
    case SCRIPT_ESCAPED_END_TAG_OPEN:
    case SCRIPT_ESCAPED_END_TAG_NAME:
    case SCRIPT_DOUBLE_ESCAPED_START:
    case SCRIPT_DOUBLE_ESCAPED:
    case SCRIPT_DOUBLE_ESCAPED_DASH:
    case SCRIPT_DOUBLE_ESCAPED_DASH_DASH:
    case SCRIPT_DOUBLE_ESCAPED_LT:
    case SCRIPT_DOUBLE_ESCAPED_END:
      error->v.tokenizer.state = ERR_TOKENIZER_SCRIPT;
      break;
    case TAG_OPEN:
    case END_TAG_OPEN:
    case TAG_NAME:
    case BEFORE_ATTR_NAME:
      error->v.tokenizer.state = ERR_TOKENIZER_TAG;
      break;
    case SELF_CLOSING_START_TAG:
      error->v.tokenizer.state = ERR_TOKENIZER_SELF_CLOSING_TAG;
      break;
    case ATTR_NAME:
    case AFTER_ATTR_NAME:
    case BEFORE_ATTR_VALUE:
      error->v.tokenizer.state = ERR_TOKENIZER_ATTR_NAME;
      break;
    case ATTR_VALUE_DOUBLE_QUOTED:
    case ATTR_VALUE_SINGLE_QUOTED:
    case ATTR_VALUE_UNQUOTED:
    case AFTER_ATTR_VALUE_QUOTED:
      error->v.tokenizer.state = ERR_TOKENIZER_ATTR_VALUE;
      break;
    case BOGUS_COMMENT:
    case COMMENT_START:
    case COMMENT_START_DASH:
    case COMMENT:
    case COMMENT_END_DASH:
    case COMMENT_END:
    case COMMENT_END_BANG:
      error->v.tokenizer.state = ERR_TOKENIZER_COMMENT;
      break;
    case MARKUP_DECLARATION:
    case DOCTYPE:
    case BEFORE_DOCTYPE_NAME:
    case DOCTYPE_NAME:
    case AFTER_DOCTYPE_NAME:
    case AFTER_DOCTYPE_PUBLIC_KEYWORD:
    case BEFORE_DOCTYPE_PUBLIC_ID:
    case DOCTYPE_PUBLIC_ID_DOUBLE_QUOTED:
    case DOCTYPE_PUBLIC_ID_SINGLE_QUOTED:
    case AFTER_DOCTYPE_PUBLIC_ID:
    case BETWEEN_DOCTYPE_PUBLIC_SYSTEM_ID:
    case AFTER_DOCTYPE_SYSTEM_KEYWORD:
    case BEFORE_DOCTYPE_SYSTEM_ID:
    case DOCTYPE_SYSTEM_ID_DOUBLE_QUOTED:
    case DOCTYPE_SYSTEM_ID_SINGLE_QUOTED:
    case AFTER_DOCTYPE_SYSTEM_ID:
    case BOGUS_DOCTYPE:
      error->v.tokenizer.state = ERR_TOKENIZER_DOCTYPE;
      break;
    case CDATA:
      error->v.tokenizer.state = ERR_TOKENIZER_CDATA;
      break;
  }
}

static bool is_alpha(int c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static int ensure_lowercase(int c) {
  return c >= 'A' && c <= 'Z' ? c + 0x20 : c;
}

static GTokenType get_char_token_type(int c) {
  switch (c) {
    case '\t':
    case '\n':
    case '\r':
    case '\f':
    case ' ':
      return TOKEN_WHITESPACE;
    case 0:
      debug("Emitted null byte.\n");
      return TOKEN_NULL;
    case -1:
      return TOKEN_EOF;
    default:
      return TOKEN_CHARACTER;
  }
}

static void clear_temporary_buffer(Parser* parser) {
  TokenizerState* tokenizer = parser->_tokenizer_state;
  assert(!tokenizer->_temporary_buffer_emit);
  utf8iterator_mark(&tokenizer->_input);
  string_buffer_destroy(parser, &tokenizer->_temporary_buffer);
  string_buffer_init(parser, &tokenizer->_temporary_buffer);
  string_buffer_destroy(parser, &tokenizer->_script_data_buffer);
  string_buffer_init(parser, &tokenizer->_script_data_buffer);
}

static void append_char_to_temporary_buffer(Parser* parser, int codepoint) {
  string_buffer_append_codepoint(parser, codepoint, &parser->_tokenizer_state->_temporary_buffer);
}

#ifndef NDEBUG
static bool temporary_buffer_equals(Parser* parser, const char* text) {
  StringBuffer* buffer = &parser->_tokenizer_state->_temporary_buffer;
  int text_len = strlen(text);
  return text_len == buffer->length &&
      memcmp(buffer->data, text, text_len) == 0;
}
#endif

static void doc_type_state_init(Parser* parser) {
  TokenDocType* doc_type_state = &parser->_tokenizer_state->_doc_type_state;
  doc_type_state->name = NULL;
  doc_type_state->public_identifier = NULL;
  doc_type_state->system_identifier = NULL;
  doc_type_state->force_quirks = false;
  doc_type_state->has_public_identifier = false;
  doc_type_state->has_system_identifier = false;
}

static void reset_token_start_point(TokenizerState* tokenizer) {
  tokenizer->_token_start = utf8iterator_get_char_pointer(&tokenizer->_input);
  utf8iterator_get_position(&tokenizer->_input, &tokenizer->_token_start_pos);
}

static void reset_tag_buffer_start_point(Parser* parser) {
  TokenizerState* tokenizer = parser->_tokenizer_state;
  TagState* tag_state = &tokenizer->_tag_state;

  utf8iterator_get_position(&tokenizer->_input, &tag_state->_start_pos);
  tag_state->_original_text = utf8iterator_get_char_pointer(&tokenizer->_input);
}

static void finish_temporary_buffer(Parser* parser, const char** output) {
  TokenizerState* tokenizer = parser->_tokenizer_state;
  *output = string_buffer_to_string(parser, &tokenizer->_temporary_buffer);
  clear_temporary_buffer(parser);
}

static void finish_token(Parser* parser, Token* token) {
  TokenizerState* tokenizer = parser->_tokenizer_state;
  if (!tokenizer->_reconsume_current_input) {
    utf8iterator_next(&tokenizer->_input);
  }

  token->position = tokenizer->_token_start_pos;
  token->original_text.data = tokenizer->_token_start;
  reset_token_start_point(tokenizer);
  token->original_text.length = tokenizer->_token_start - token->original_text.data;
  if (token->original_text.length > 0 && token->original_text.data[token->original_text.length - 1] == '\r') {
    --token->original_text.length;
  }
}

static void finish_doctype_public_id(Parser* parser) {
  TokenDocType* doc_type_state = &parser->_tokenizer_state->_doc_type_state;
  parser_deallocate(parser, (void*) doc_type_state->public_identifier);
  finish_temporary_buffer(parser, &doc_type_state->public_identifier);
  doc_type_state->has_public_identifier = true;
}

static void finish_doctype_system_id(Parser* parser) {
  TokenDocType* doc_type_state = &parser->_tokenizer_state->_doc_type_state;
  parser_deallocate(parser, (void*) doc_type_state->system_identifier);
  finish_temporary_buffer(parser, &doc_type_state->system_identifier);
  doc_type_state->has_system_identifier = true;
}

static void emit_char(Parser* parser, int c, Token* output) {
  output->type = get_char_token_type(c);
  output->v.character = c;
  finish_token(parser, output);
}

static StateResult emit_replacement_char(Parser* parser, Token* output) {
  add_parse_error(parser, ERR_UTF8_NULL);
  emit_char(parser, kUtf8ReplacementChar, output);
  return RETURN_ERROR;
}

static StateResult emit_eof(Parser* parser, Token* output) {
  emit_char(parser, -1, output);
  return RETURN_SUCCESS;
}

static bool emit_current_char(Parser* parser, Token* output) {
  emit_char(parser, utf8iterator_current(&parser->_tokenizer_state->_input), output);
  return RETURN_SUCCESS;
}

static void emit_doctype(Parser* parser, Token* output) {
  output->type = TOKEN_DOCTYPE;
  output->v.doc_type = parser->_tokenizer_state->_doc_type_state;
  finish_token(parser, output);
  doc_type_state_init(parser);
}

static void mark_tag_state_as_empty(TagState* tag_state) {
#ifndef NDEBUG
  tag_state->_attributes = kEmptyVector;
#endif
}

static StateResult emit_current_tag(Parser* parser, Token* output) {
  TagState* tag_state = &parser->_tokenizer_state->_tag_state;
  if (tag_state->_is_start_tag) {
    output->type = TOKEN_START_TAG;
    output->v.start_tag.tag = tag_state->_tag;
    output->v.start_tag.attributes = tag_state->_attributes;
    output->v.start_tag.is_self_closing = tag_state->_is_self_closing;
    tag_state->_last_start_tag = tag_state->_tag;
    mark_tag_state_as_empty(tag_state);
    debug("Emitted start tag %s.\n", normalized_tagname(tag_state->_tag));
  } else {
    output->type = TOKEN_END_TAG;
    output->v.end_tag = tag_state->_tag;
    for (int i = 0; i < tag_state->_attributes.length; ++i) {
      destroy_attribute(parser, tag_state->_attributes.data[i]);
    }
    parser_deallocate(parser, tag_state->_attributes.data);
    mark_tag_state_as_empty(tag_state);
    debug("Emitted end tag %s.\n", normalized_tagname(tag_state->_tag));
  }
  string_buffer_destroy(parser, &tag_state->_buffer);
  finish_token(parser, output);
  debug("Original text = %.*s.\n", output->original_text.length, output->original_text.data);
  assert(output->original_text.length >= 2);
  assert(output->original_text.data[0] == '<');
  assert(output->original_text.data[output->original_text.length - 1] == '>');
  return RETURN_SUCCESS;
}

static void abandon_current_tag(Parser* parser) {
  TagState* tag_state = &parser->_tokenizer_state->_tag_state;
  for (int i = 0; i < tag_state->_attributes.length; ++i) {
    destroy_attribute(parser, tag_state->_attributes.data[i]);
  }
  parser_deallocate(parser, tag_state->_attributes.data);
  mark_tag_state_as_empty(tag_state);
  string_buffer_destroy(parser, &tag_state->_buffer);
  debug("Abandoning current tag.\n");
}

static StateResult emit_char_ref(Parser* parser, int additional_allowed_char, bool is_in_attribute, Token* output) {
  TokenizerState* tokenizer = parser->_tokenizer_state;
  OneOrTwoCodepoints char_ref;
  bool status = consume_char_ref(parser, &tokenizer->_input, additional_allowed_char, false, &char_ref);
  if (char_ref.first != kNoChar) {
    tokenizer->_reconsume_current_input = true;
    emit_char(parser, char_ref.first, output);
    tokenizer->_buffered_emit_char = char_ref.second;
  } else {
    emit_char(parser, '&', output);
  }
  return status ? RETURN_SUCCESS : RETURN_ERROR;
}

static StateResult emit_comment(Parser* parser, Token* output) {
  output->type = TOKEN_COMMENT;
  finish_temporary_buffer(parser, &output->v.text);
  finish_token(parser, output);
  return RETURN_SUCCESS;
}

static bool maybe_emit_from_temporary_buffer(Parser* parser, Token* output) {
  TokenizerState* tokenizer = parser->_tokenizer_state;
  const char* c = tokenizer->_temporary_buffer_emit;
  StringBuffer* buffer = &tokenizer->_temporary_buffer;

  if (!c || c >= buffer->data + buffer->length) {
    tokenizer->_temporary_buffer_emit = NULL;
    return false;
  }

  assert(*c == utf8iterator_current(&tokenizer->_input));
  bool saved_reconsume_state = tokenizer->_reconsume_current_input;
  tokenizer->_reconsume_current_input = false;
  emit_char(parser, *c, output);
  ++tokenizer->_temporary_buffer_emit;
  tokenizer->_reconsume_current_input = saved_reconsume_state;
  return true;
}

static bool emit_temporary_buffer(Parser* parser, Token* output) {
  TokenizerState* tokenizer = parser->_tokenizer_state;
  assert(tokenizer->_temporary_buffer.data);
  utf8iterator_reset(&tokenizer->_input);
  tokenizer->_temporary_buffer_emit = tokenizer->_temporary_buffer.data;
  return maybe_emit_from_temporary_buffer(parser, output);
}

static void append_char_to_tag_buffer(Parser* parser, int codepoint, bool reinitilize_position_on_first) {
  StringBuffer* buffer = &parser->_tokenizer_state->_tag_state._buffer;
  if (buffer->length == 0 && reinitilize_position_on_first) {
    reset_tag_buffer_start_point(parser);
  }
  string_buffer_append_codepoint(parser, codepoint, buffer);
}

static void initialize_tag_buffer(Parser* parser) {
  TokenizerState* tokenizer = parser->_tokenizer_state;
  TagState* tag_state = &tokenizer->_tag_state;

  string_buffer_init(parser, &tag_state->_buffer);
  reset_tag_buffer_start_point(parser);
}

static void start_new_tag(Parser* parser, bool is_start_tag) {
  TokenizerState* tokenizer = parser->_tokenizer_state;
  TagState* tag_state = &tokenizer->_tag_state;
  int c = utf8iterator_current(&tokenizer->_input);
  assert(is_alpha(c));
  c = ensure_lowercase(c);
  assert(is_alpha(c));

  initialize_tag_buffer(parser);
  string_buffer_append_codepoint(parser, c, &tag_state->_buffer);

  assert(tag_state->_attributes.data == NULL);
  vector_init(parser, 4, &tag_state->_attributes);
  tag_state->_drop_next_attr_value = false;
  tag_state->_is_start_tag = is_start_tag;
  tag_state->_is_self_closing = false;
  debug("Starting new tag.\n");
}

static void copy_over_tag_buffer(Parser* parser, const char** output) {
  TokenizerState* tokenizer = parser->_tokenizer_state;
  TagState* tag_state = &tokenizer->_tag_state;
  *output = string_buffer_to_string(parser, &tag_state->_buffer);
}

static void copy_over_original_tag_text(Parser* parser, StringPiece* original_text, SrcPosition* start_pos, SrcPosition* end_pos){
  TokenizerState* tokenizer = parser->_tokenizer_state;
  TagState* tag_state = &tokenizer->_tag_state;

  original_text->data = tag_state->_original_text;
  original_text->length = utf8iterator_get_char_pointer(&tokenizer->_input) - tag_state->_original_text;
  if (original_text->data[original_text->length - 1] == '\r') {
    --original_text->length;
  }
  *start_pos = tag_state->_start_pos;
  utf8iterator_get_position(&tokenizer->_input, end_pos);
}

static void reinitialize_tag_buffer(Parser* parser) {
  parser_deallocate(parser, parser->_tokenizer_state->_tag_state._buffer.data);
  initialize_tag_buffer(parser);
}

static void finish_tag_name(Parser* parser) {
  TokenizerState* tokenizer = parser->_tokenizer_state;
  TagState* tag_state = &tokenizer->_tag_state;

  const char* temp;
  copy_over_tag_buffer(parser, &temp);
  tag_state->_tag = tag_enum(temp);
  reinitialize_tag_buffer(parser);
  parser_deallocate(parser, (void*) temp);
}

// Adds an ERR_DUPLICATE_ATTR parse error to the parser's error struct.
static void add_duplicate_attr_error(Parser* parser, const char* attr_name, int original_index, int new_index) {
  Error* error = add_error(parser);
  if (!error) {
    return;
  }
  TagState* tag_state = &parser->_tokenizer_state->_tag_state;
  error->type = ERR_DUPLICATE_ATTR;
  error->position = tag_state->_start_pos;
  error->original_text = tag_state->_original_text;
  error->v.duplicate_attr.original_index = original_index;
  error->v.duplicate_attr.new_index = new_index;
  copy_over_tag_buffer(parser, &error->v.duplicate_attr.name);
  reinitialize_tag_buffer(parser);
}

static bool finish_attribute_name(Parser* parser) {
  TokenizerState* tokenizer = parser->_tokenizer_state;
  TagState* tag_state = &tokenizer->_tag_state;
  tag_state->_drop_next_attr_value = false;
  assert(tag_state->_attributes.data);
  assert(tag_state->_attributes.capacity);

  Vector* /* Attribute* */ attributes = &tag_state->_attributes;
  for (int i = 0; i < attributes->length; ++i) {
    Attribute* attr = attributes->data[i];
    if (strlen(attr->name) == tag_state->_buffer.length &&
        memcmp(attr->name, tag_state->_buffer.data, tag_state->_buffer.length) == 0) {
      add_duplicate_attr_error(parser, attr->name, i, attributes->length);
      tag_state->_drop_next_attr_value = true;
      return false;
    }
  }

  Attribute* attr = parser_allocate(parser, sizeof(Attribute));
  attr->attr_namespace = ATTR_NAMESPACE_NONE;
  copy_over_tag_buffer(parser, &attr->name);
  copy_over_original_tag_text(parser, &attr->original_name, &attr->name_start, &attr->name_end);
  attr->value = copy_stringz(parser, "");
  copy_over_original_tag_text(parser, &attr->original_value, &attr->name_start, &attr->name_end);
  vector_add(parser, attr, attributes);
  reinitialize_tag_buffer(parser);
  return true;
}

static void finish_attribute_value(Parser* parser) {
  TagState* tag_state = &parser->_tokenizer_state->_tag_state;
  if (tag_state->_drop_next_attr_value) {
    tag_state->_drop_next_attr_value = false;
    return;
  }

  Attribute* attr = tag_state->_attributes.data[tag_state->_attributes.length - 1];
  parser_deallocate(parser, (void*) attr->value);
  copy_over_tag_buffer(parser, &attr->value);
  copy_over_original_tag_text(parser, &attr->original_value, &attr->value_start, &attr->value_end);
  reinitialize_tag_buffer(parser);
}

static bool is_appropriate_end_tag(Parser* parser) {
  TagState* tag_state = &parser->_tokenizer_state->_tag_state;
  assert(!tag_state->_is_start_tag);
  string_buffer_append_codepoint(parser, '\0', &tag_state->_buffer);
  --tag_state->_buffer.length;
  return tag_state->_last_start_tag != TAG_LAST && tag_state->_last_start_tag == tag_enum(tag_state->_buffer.data);
}

void tokenizer_state_init(Parser* parser, const char* text, size_t text_length) {
  TokenizerState* tokenizer = parser_allocate(parser, sizeof(TokenizerState));
  parser->_tokenizer_state = tokenizer;
  tokenizer_set_state(parser, DATA);
  tokenizer->_reconsume_current_input = false;
  tokenizer->_is_current_node_foreign = false;
  tokenizer->_tag_state._last_start_tag = TAG_LAST;

  tokenizer->_buffered_emit_char = kNoChar;
  string_buffer_init(parser, &tokenizer->_temporary_buffer);
  tokenizer->_temporary_buffer_emit = NULL;

  mark_tag_state_as_empty(&tokenizer->_tag_state);

  string_buffer_init(parser, &tokenizer->_script_data_buffer);
  tokenizer->_token_start = text;
  utf8iterator_init(parser, text, text_length, &tokenizer->_input);
  utf8iterator_get_position(&tokenizer->_input, &tokenizer->_token_start_pos);
  doc_type_state_init(parser);
}

void tokenizer_state_destroy(Parser* parser) {
  TokenizerState* tokenizer = parser->_tokenizer_state;
  assert(tokenizer->_doc_type_state.name == NULL);
  assert(tokenizer->_doc_type_state.public_identifier == NULL);
  assert(tokenizer->_doc_type_state.system_identifier == NULL);
  string_buffer_destroy(parser, &tokenizer->_temporary_buffer);
  string_buffer_destroy(parser, &tokenizer->_script_data_buffer);
  parser_deallocate(parser, tokenizer);
}

void tokenizer_set_state(Parser* parser, TokenizerEnum state) {
  parser->_tokenizer_state->_state = state;
}

void tokenizer_set_is_current_node_foreign(Parser* parser, bool is_foreign) {
  if (is_foreign != parser->_tokenizer_state->_is_current_node_foreign) {
    debug("Toggling is_current_node_foreign to %s.\n", is_foreign ? "true" : "false");
  }
  parser->_tokenizer_state->_is_current_node_foreign = is_foreign;
}

static StateResult handle_data_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '&':
      tokenizer_set_state(parser, CHAR_REF_IN_DATA);
      tokenizer->_reconsume_current_input = true;
      return NEXT_CHAR;
    case '<':
      tokenizer_set_state(parser, TAG_OPEN);
      clear_temporary_buffer(parser);
      append_char_to_temporary_buffer(parser, '<');
      return NEXT_CHAR;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      emit_char(parser, c, output);
      return RETURN_ERROR;
    default:
      return emit_current_char(parser, output);
  }
}

static StateResult handle_char_ref_in_data_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  tokenizer_set_state(parser, DATA);
  return emit_char_ref(parser, ' ', false, output);
}

static StateResult handle_rcdata_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '&':
      tokenizer_set_state(parser, CHAR_REF_IN_RCDATA);
      tokenizer->_reconsume_current_input = true;
      return NEXT_CHAR;
    case '<':
      tokenizer_set_state(parser, RCDATA_LT);
      clear_temporary_buffer(parser);
      append_char_to_temporary_buffer(parser, '<');
      return NEXT_CHAR;
    case '\0':
      return emit_replacement_char(parser, output);
    case -1:
      return emit_eof(parser, output);
    default:
      return emit_current_char(parser, output);
  }
}

static StateResult handle_char_ref_in_rcdata_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  tokenizer_set_state(parser, RCDATA);
  return emit_char_ref(parser, ' ', false, output);
}

static StateResult handle_rawtext_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '<':
      tokenizer_set_state(parser, RAWTEXT_LT);
      clear_temporary_buffer(parser);
      append_char_to_temporary_buffer(parser, '<');
      return NEXT_CHAR;
    case '\0':
      return emit_replacement_char(parser, output);
    case -1:
      return emit_eof(parser, output);
    default:
      return emit_current_char(parser, output);
  }
}

static StateResult handle_script_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '<':
      tokenizer_set_state(parser, SCRIPT_LT);
      clear_temporary_buffer(parser);
      append_char_to_temporary_buffer(parser, '<');
      return NEXT_CHAR;
    case '\0':
      return emit_replacement_char(parser, output);
    case -1:
      return emit_eof(parser, output);
    default:
      return emit_current_char(parser, output);
  }
}

static StateResult handle_plaintext_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '\0':
      return emit_replacement_char(parser, output);
    case -1:
      return emit_eof(parser, output);
    default:
      return emit_current_char(parser, output);
  }
}

static StateResult handle_tag_open_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  assert(temporary_buffer_equals(parser, "<"));
  switch (c) {
    case '!':
      tokenizer_set_state(parser, MARKUP_DECLARATION);
      clear_temporary_buffer(parser);
      return NEXT_CHAR;
    case '/':
      tokenizer_set_state(parser, END_TAG_OPEN);
      append_char_to_temporary_buffer(parser, '/');
      return NEXT_CHAR;
    case '?':
      tokenizer_set_state(parser, BOGUS_COMMENT);
      clear_temporary_buffer(parser);
      append_char_to_temporary_buffer(parser, '?');
      add_parse_error(parser, ERR_TAG_STARTS_WITH_QUESTION);
      return NEXT_CHAR;
    default:
      if (is_alpha(c)) {
        tokenizer_set_state(parser, TAG_NAME);
        start_new_tag(parser, true);
        return NEXT_CHAR;
      } else {
        add_parse_error(parser, ERR_TAG_INVALID);
        tokenizer_set_state(parser, DATA);
        emit_temporary_buffer(parser, output);
        return RETURN_ERROR;
      }
  }
}

static StateResult handle_end_tag_open_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  assert(temporary_buffer_equals(parser, "</"));
  switch (c) {
    case '>':
      add_parse_error(parser, ERR_CLOSE_TAG_EMPTY);
      tokenizer_set_state(parser, DATA);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_CLOSE_TAG_EOF);
      tokenizer_set_state(parser, DATA);
      return emit_temporary_buffer(parser, output);
    default:
      if (is_alpha(c)) {
        tokenizer_set_state(parser,TAG_NAME);
        start_new_tag(parser, false);
      } else {
        add_parse_error(parser, ERR_CLOSE_TAG_INVALID);
        tokenizer_set_state(parser, BOGUS_COMMENT);
        clear_temporary_buffer(parser);
        append_char_to_temporary_buffer(parser, c);
      }
      return NEXT_CHAR;
  }
}

static StateResult handle_tag_name_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      finish_tag_name(parser);
      tokenizer_set_state(parser, BEFORE_ATTR_NAME);
      return NEXT_CHAR;
    case '/':
      finish_tag_name(parser);
      tokenizer_set_state(parser, SELF_CLOSING_START_TAG);
      return NEXT_CHAR;
    case '>':
      finish_tag_name(parser);
      tokenizer_set_state(parser, DATA);
      return emit_current_tag(parser, output);
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      append_char_to_tag_buffer(parser, kUtf8ReplacementChar, true);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_TAG_EOF);
      abandon_current_tag(parser);
      tokenizer_set_state(parser, DATA);
      return NEXT_CHAR;
    default:
      append_char_to_tag_buffer(parser, ensure_lowercase(c), true);
      return NEXT_CHAR;
  }
}

static StateResult handle_rcdata_lt_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  assert(temporary_buffer_equals(parser, "<"));
  if (c == '/') {
    tokenizer_set_state(parser, RCDATA_END_TAG_OPEN);
    append_char_to_temporary_buffer(parser, '/');
    return NEXT_CHAR;
  } else {
    tokenizer_set_state(parser, RCDATA);
    tokenizer->_reconsume_current_input = true;
    return emit_temporary_buffer(parser, output);
  }
}

static StateResult handle_rcdata_end_tag_open_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  assert(temporary_buffer_equals(parser, "</"));
  if (is_alpha(c)) {
    tokenizer_set_state(parser, RCDATA_END_TAG_NAME);
    start_new_tag(parser, false);
    append_char_to_temporary_buffer(parser, c);
    return NEXT_CHAR;
  } else {
    tokenizer_set_state(parser, RCDATA);
    return emit_temporary_buffer(parser, output);
  }
  return true;
}

static StateResult handle_rcdata_end_tag_name_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  assert(tokenizer->_temporary_buffer.length >= 2);
  if (is_alpha(c)) {
    append_char_to_tag_buffer(parser, ensure_lowercase(c), true);
    append_char_to_temporary_buffer(parser, c);
    return NEXT_CHAR;
  } else if (is_appropriate_end_tag(parser)) {
    switch (c) {
      case '\t':
      case '\n':
      case '\f':
      case ' ':
        finish_tag_name(parser);
        tokenizer_set_state(parser, BEFORE_ATTR_NAME);
        return NEXT_CHAR;
      case '/':
        finish_tag_name(parser);
        tokenizer_set_state(parser, SELF_CLOSING_START_TAG);
        return NEXT_CHAR;
      case '>':
        finish_tag_name(parser);
        tokenizer_set_state(parser, DATA);
        return emit_current_tag(parser, output);
    }
  }
  tokenizer_set_state(parser, RCDATA);
  abandon_current_tag(parser);
  return emit_temporary_buffer(parser, output);
}

static StateResult handle_rawtext_lt_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  assert(temporary_buffer_equals(parser, "<"));
  if (c == '/') {
    tokenizer_set_state(parser, RAWTEXT_END_TAG_OPEN);
    append_char_to_temporary_buffer(parser, '/');
    return NEXT_CHAR;
  } else {
    tokenizer_set_state(parser, RAWTEXT);
    tokenizer->_reconsume_current_input = true;
    return emit_temporary_buffer(parser, output);
  }
}

static StateResult handle_rawtext_end_tag_open_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  assert(temporary_buffer_equals(parser, "</"));
  if (is_alpha(c)) {
    tokenizer_set_state(parser, RAWTEXT_END_TAG_NAME);
    start_new_tag(parser, false);
    append_char_to_temporary_buffer(parser, c);
    return NEXT_CHAR;
  } else {
    tokenizer_set_state(parser, RAWTEXT);
    return emit_temporary_buffer(parser, output);
  }
}

static StateResult handle_rawtext_end_tag_name_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  assert(tokenizer->_temporary_buffer.length >= 2);
  debug("Last end tag: %*s\n", (int) tokenizer->_tag_state._buffer.length, tokenizer->_tag_state._buffer.data);
  if (is_alpha(c)) {
    append_char_to_tag_buffer(parser, ensure_lowercase(c), true);
    append_char_to_temporary_buffer(parser, c);
    return NEXT_CHAR;
  } else if (is_appropriate_end_tag(parser)) {
    debug("Is an appropriate end tag.\n");
    switch (c) {
      case '\t':
      case '\n':
      case '\f':
      case ' ':
        finish_tag_name(parser);
        tokenizer_set_state(parser, BEFORE_ATTR_NAME);
        return NEXT_CHAR;
      case '/':
        finish_tag_name(parser);
        tokenizer_set_state(parser, SELF_CLOSING_START_TAG);
        return NEXT_CHAR;
      case '>':
        finish_tag_name(parser);
        tokenizer_set_state(parser, DATA);
        return emit_current_tag(parser, output);
    }
  }
  tokenizer_set_state(parser, RAWTEXT);
  abandon_current_tag(parser);
  return emit_temporary_buffer(parser, output);
}

static StateResult handle_script_lt_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  assert(temporary_buffer_equals(parser, "<"));
  if (c == '/') {
    tokenizer_set_state(parser, SCRIPT_END_TAG_OPEN);
    append_char_to_temporary_buffer(parser, '/');
    return NEXT_CHAR;
  } else if (c == '!') {
    tokenizer_set_state(parser, SCRIPT_ESCAPED_START);
    append_char_to_temporary_buffer(parser, '!');
    return emit_temporary_buffer(parser, output);
  } else {
    tokenizer_set_state(parser, SCRIPT);
    tokenizer->_reconsume_current_input = true;
    return emit_temporary_buffer(parser, output);
  }
}

static StateResult handle_script_end_tag_open_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  assert(temporary_buffer_equals(parser, "</"));
  if (is_alpha(c)) {
    tokenizer_set_state(parser, SCRIPT_END_TAG_NAME);
    start_new_tag(parser, false);
    append_char_to_temporary_buffer(parser, c);
    return NEXT_CHAR;
  } else {
    tokenizer_set_state(parser, SCRIPT);
    return emit_temporary_buffer(parser, output);
  }
}

static StateResult handle_script_end_tag_name_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  assert(tokenizer->_temporary_buffer.length >= 2);
  if (is_alpha(c)) {
    append_char_to_tag_buffer(parser, ensure_lowercase(c), true);
    append_char_to_temporary_buffer(parser, c);
    return NEXT_CHAR;
  } else if (is_appropriate_end_tag(parser)) {
    switch (c) {
      case '\t':
      case '\n':
      case '\f':
      case ' ':
        finish_tag_name(parser);
        tokenizer_set_state(parser, BEFORE_ATTR_NAME);
        return NEXT_CHAR;
      case '/':
        finish_tag_name(parser);
        tokenizer_set_state(parser, SELF_CLOSING_START_TAG);
        return NEXT_CHAR;
      case '>':
        finish_tag_name(parser);
        tokenizer_set_state(parser, DATA);
        return emit_current_tag(parser, output);
    }
  }
  tokenizer_set_state(parser, SCRIPT);
  abandon_current_tag(parser);
  return emit_temporary_buffer(parser, output);
}

static StateResult handle_script_escaped_start_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  if (c == '-') {
    tokenizer_set_state(parser, SCRIPT_ESCAPED_START_DASH);
    return emit_current_char(parser, output);
  } else {
    tokenizer_set_state(parser, SCRIPT);
    tokenizer->_reconsume_current_input = true;
    return NEXT_CHAR;
  }
}

static StateResult handle_script_escaped_start_dash_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  if (c == '-') {
    tokenizer_set_state(parser, SCRIPT_ESCAPED_DASH_DASH);
    return emit_current_char(parser, output);
  } else {
    tokenizer_set_state(parser, SCRIPT);
    tokenizer->_reconsume_current_input = true;
    return NEXT_CHAR;
  }
}

static StateResult handle_script_escaped_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '-':
      tokenizer_set_state(parser, SCRIPT_ESCAPED_DASH);
      return emit_current_char(parser, output);
    case '<':
      tokenizer_set_state(parser, SCRIPT_ESCAPED_LT);
      clear_temporary_buffer(parser);
      append_char_to_temporary_buffer(parser, c);
      return NEXT_CHAR;
    case '\0':
      return emit_replacement_char(parser, output);
    case -1:
      add_parse_error(parser, ERR_SCRIPT_EOF);
      return emit_eof(parser, output);
    default:
      return emit_current_char(parser, output);
  }
}

static StateResult handle_script_escaped_dash_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '-':
      tokenizer_set_state(parser, SCRIPT_ESCAPED_DASH_DASH);
      return emit_current_char(parser, output);
    case '<':
      tokenizer_set_state(parser, SCRIPT_ESCAPED_LT);
      clear_temporary_buffer(parser);
      append_char_to_temporary_buffer(parser, c);
      return NEXT_CHAR;
    case '\0':
      tokenizer_set_state(parser, SCRIPT_ESCAPED);
      return emit_replacement_char(parser, output);
    case -1:
      add_parse_error(parser, ERR_SCRIPT_EOF);
      tokenizer_set_state(parser, DATA);
      return NEXT_CHAR;
    default:
      tokenizer_set_state(parser, SCRIPT_ESCAPED);
      return emit_current_char(parser, output);
  }
}

static StateResult handle_script_escaped_dash_dash_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '-':
      return emit_current_char(parser, output);
    case '<':
      tokenizer_set_state(parser, SCRIPT_ESCAPED_LT);
      clear_temporary_buffer(parser);
      append_char_to_temporary_buffer(parser, c);
      return NEXT_CHAR;
    case '>':
      tokenizer_set_state(parser, SCRIPT);
      return emit_current_char(parser, output);
    case '\0':
      tokenizer_set_state(parser, SCRIPT_ESCAPED);
      return emit_replacement_char(parser, output);
    case -1:
      add_parse_error(parser, ERR_SCRIPT_EOF);
      tokenizer_set_state(parser, DATA);
      return NEXT_CHAR;
    default:
      tokenizer_set_state(parser, SCRIPT_ESCAPED);
      return emit_current_char(parser, output);
  }
}

static StateResult handle_script_escaped_lt_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  assert(temporary_buffer_equals(parser, "<"));
  assert(!tokenizer->_script_data_buffer.length);
  if (c == '/') {
    tokenizer_set_state(parser, SCRIPT_ESCAPED_END_TAG_OPEN);
    append_char_to_temporary_buffer(parser, c);
    return NEXT_CHAR;
  } else if (is_alpha(c)) {
    tokenizer_set_state(parser, SCRIPT_DOUBLE_ESCAPED_START);
    append_char_to_temporary_buffer(parser, c);
    string_buffer_append_codepoint(parser, ensure_lowercase(c), &tokenizer->_script_data_buffer);
    return emit_temporary_buffer(parser, output);
  } else {
    tokenizer_set_state(parser, SCRIPT_ESCAPED);
    return emit_temporary_buffer(parser, output);
  }
}

static StateResult handle_script_escaped_end_tag_open_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  assert(temporary_buffer_equals(parser, "</"));
  if (is_alpha(c)) {
    tokenizer_set_state(parser, SCRIPT_ESCAPED_END_TAG_NAME);
    start_new_tag(parser, false);
    append_char_to_temporary_buffer(parser, c);
    return NEXT_CHAR;
  } else {
    tokenizer_set_state(parser, SCRIPT_ESCAPED);
    return emit_temporary_buffer(parser, output);
  }
}

static StateResult handle_script_escaped_end_tag_name_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  assert(tokenizer->_temporary_buffer.length >= 2);
  if (is_alpha(c)) {
    append_char_to_tag_buffer(parser, ensure_lowercase(c), true);
    append_char_to_temporary_buffer(parser, c);
    return NEXT_CHAR;
  } else if (is_appropriate_end_tag(parser)) {
    switch (c) {
      case '\t':
      case '\n':
      case '\f':
      case ' ':
        finish_tag_name(parser);
        tokenizer_set_state(parser, BEFORE_ATTR_NAME);
        return NEXT_CHAR;
      case '/':
        finish_tag_name(parser);
        tokenizer_set_state(parser, SELF_CLOSING_START_TAG);
        return NEXT_CHAR;
      case '>':
        finish_tag_name(parser);
        tokenizer_set_state(parser, DATA);
        return emit_current_tag(parser, output);
    }
  }
  tokenizer_set_state(parser, SCRIPT_ESCAPED);
  abandon_current_tag(parser);
  return emit_temporary_buffer(parser, output);
}

static StateResult handle_script_double_escaped_start_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
    case '/':
    case '>':
      tokenizer_set_state(parser, string_equals(&kScriptTag, (StringPiece*) &tokenizer->_script_data_buffer) ? SCRIPT_DOUBLE_ESCAPED : SCRIPT_ESCAPED);
      return emit_current_char(parser, output);
    default:
      if (is_alpha(c)) {
        string_buffer_append_codepoint(parser, ensure_lowercase(c), &tokenizer->_script_data_buffer);
        return emit_current_char(parser, output);
      } else {
        tokenizer_set_state(parser, SCRIPT_ESCAPED);
        tokenizer->_reconsume_current_input = true;
        return NEXT_CHAR;
      }
  }
}

static StateResult handle_script_double_escaped_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '-':
      tokenizer_set_state(parser, SCRIPT_DOUBLE_ESCAPED_DASH);
      return emit_current_char(parser, output);
    case '<':
      tokenizer_set_state(parser, SCRIPT_DOUBLE_ESCAPED_LT);
      return emit_current_char(parser, output);
    case '\0':
      return emit_replacement_char(parser, output);
    case -1:
      add_parse_error(parser, ERR_SCRIPT_EOF);
      tokenizer_set_state(parser, DATA);
      return NEXT_CHAR;
    default:
      return emit_current_char(parser, output);
  }
}

static StateResult handle_script_double_escaped_dash_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '-':
      tokenizer_set_state(parser, SCRIPT_DOUBLE_ESCAPED_DASH_DASH);
      return emit_current_char(parser, output);
    case '<':
      tokenizer_set_state(parser, SCRIPT_DOUBLE_ESCAPED_LT);
      return emit_current_char(parser, output);
    case '\0':
      tokenizer_set_state(parser, SCRIPT_DOUBLE_ESCAPED);
      return emit_replacement_char(parser, output);
    case -1:
      add_parse_error(parser, ERR_SCRIPT_EOF);
      tokenizer_set_state(parser, DATA);
      return NEXT_CHAR;
    default:
      tokenizer_set_state(parser, SCRIPT_DOUBLE_ESCAPED);
      return emit_current_char(parser, output);
  }
}

static StateResult handle_script_double_escaped_dash_dash_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '-':
      return emit_current_char(parser, output);
    case '<':
      tokenizer_set_state(parser, SCRIPT_DOUBLE_ESCAPED_LT);
      return emit_current_char(parser, output);
    case '>':
      tokenizer_set_state(parser, SCRIPT);
      return emit_current_char(parser, output);
    case '\0':
      tokenizer_set_state(parser, SCRIPT_DOUBLE_ESCAPED);
      return emit_replacement_char(parser, output);
    case -1:
      add_parse_error(parser, ERR_SCRIPT_EOF);
      tokenizer_set_state(parser, DATA);
      return NEXT_CHAR;
    default:
      tokenizer_set_state(parser, SCRIPT_DOUBLE_ESCAPED);
      return emit_current_char(parser, output);
  }
}

static StateResult handle_script_double_escaped_lt_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  if (c == '/') {
    tokenizer_set_state(parser, SCRIPT_DOUBLE_ESCAPED_END);
    string_buffer_destroy(parser, &tokenizer->_script_data_buffer);
    string_buffer_init(parser, &tokenizer->_script_data_buffer);
    return emit_current_char(parser, output);
  } else {
    tokenizer_set_state(parser, SCRIPT_DOUBLE_ESCAPED);
    tokenizer->_reconsume_current_input = true;
    return NEXT_CHAR;
  }

}

static StateResult handle_script_double_escaped_end_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
    case '/':
    case '>':
      tokenizer_set_state(parser, string_equals(&kScriptTag, (StringPiece*) &tokenizer->_script_data_buffer) ? SCRIPT_ESCAPED : SCRIPT_DOUBLE_ESCAPED);
      return emit_current_char(parser, output);
    default:
      if (is_alpha(c)) {
        string_buffer_append_codepoint(parser, ensure_lowercase(c), &tokenizer->_script_data_buffer);
        return emit_current_char(parser, output);
      } else {
        tokenizer_set_state(parser, SCRIPT_DOUBLE_ESCAPED);
        tokenizer->_reconsume_current_input = true;
        return NEXT_CHAR;
      }
  }
}

static StateResult handle_before_attr_name_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      return NEXT_CHAR;
    case '/':
      tokenizer_set_state(parser, SELF_CLOSING_START_TAG);
      return NEXT_CHAR;
    case '>':
      tokenizer_set_state(parser, DATA);
      return emit_current_tag(parser, output);
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      tokenizer_set_state(parser, ATTR_NAME);
      append_char_to_temporary_buffer(parser, 0xfffd);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_ATTR_NAME_EOF);
      tokenizer_set_state(parser, DATA);
      abandon_current_tag(parser);
      return NEXT_CHAR;
    case '"':
    case '\'':
    case '<':
    case '=':
      add_parse_error(parser, ERR_ATTR_NAME_INVALID);
      // Fall through.
    default:
      tokenizer_set_state(parser, ATTR_NAME);
      append_char_to_tag_buffer(parser, ensure_lowercase(c), true);
      return NEXT_CHAR;
  }
}

static StateResult handle_attr_name_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      finish_attribute_name(parser);
      tokenizer_set_state(parser, AFTER_ATTR_NAME);
      return NEXT_CHAR;
    case '/':
      finish_attribute_name(parser);
      tokenizer_set_state(parser, SELF_CLOSING_START_TAG);
      return NEXT_CHAR;
    case '=':
      finish_attribute_name(parser);
      tokenizer_set_state(parser, BEFORE_ATTR_VALUE);
      return NEXT_CHAR;
    case '>':
      finish_attribute_name(parser);
      tokenizer_set_state(parser, DATA);
      return emit_current_tag(parser, output);
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      append_char_to_tag_buffer(parser, kUtf8ReplacementChar, true);
      return NEXT_CHAR;
    case -1:
      tokenizer_set_state(parser, DATA);
      abandon_current_tag(parser);
      add_parse_error(parser, ERR_ATTR_NAME_EOF);
      return NEXT_CHAR;
    case '"':
    case '\'':
    case '<':
      add_parse_error(parser, ERR_ATTR_NAME_INVALID);
      // Fall through.
    default:
      append_char_to_tag_buffer(parser, ensure_lowercase(c), true);
      return NEXT_CHAR;
  }
}

static StateResult handle_after_attr_name_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      return NEXT_CHAR;
    case '/':
      tokenizer_set_state(parser, SELF_CLOSING_START_TAG);
      return NEXT_CHAR;
    case '=':
      tokenizer_set_state(parser, BEFORE_ATTR_VALUE);
      return NEXT_CHAR;
    case '>':
      tokenizer_set_state(parser, DATA);
      return emit_current_tag(parser, output);
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      tokenizer_set_state(parser, ATTR_NAME);
      append_char_to_temporary_buffer(parser, 0xfffd);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_ATTR_NAME_EOF);
      tokenizer_set_state(parser, DATA);
      abandon_current_tag(parser);
      return NEXT_CHAR;
    case '"':
    case '\'':
    case '<':
      add_parse_error(parser, ERR_ATTR_NAME_INVALID);
      // Fall through.
    default:
      tokenizer_set_state(parser, ATTR_NAME);
      append_char_to_tag_buffer(parser, ensure_lowercase(c), true);
      return NEXT_CHAR;
  }
}

static StateResult handle_before_attr_value_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      return NEXT_CHAR;
    case '"':
      tokenizer_set_state(parser, ATTR_VALUE_DOUBLE_QUOTED);
      reset_tag_buffer_start_point(parser);
      return NEXT_CHAR;
    case '&':
      tokenizer_set_state(parser, ATTR_VALUE_UNQUOTED);
      tokenizer->_reconsume_current_input = true;
      return NEXT_CHAR;
    case '\'':
      tokenizer_set_state(parser, ATTR_VALUE_SINGLE_QUOTED);
      reset_tag_buffer_start_point(parser);
      return NEXT_CHAR;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      tokenizer_set_state(parser, ATTR_VALUE_UNQUOTED);
      append_char_to_tag_buffer(parser, kUtf8ReplacementChar, true);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_ATTR_UNQUOTED_EOF);
      tokenizer_set_state(parser, DATA);
      abandon_current_tag(parser);
      tokenizer->_reconsume_current_input = true;
      return NEXT_CHAR;
    case '>':
      add_parse_error(parser, ERR_ATTR_UNQUOTED_RIGHT_BRACKET);
      tokenizer_set_state(parser, DATA);
      emit_current_tag(parser, output);
      return RETURN_ERROR;
    case '<':
    case '=':
    case '`':
      add_parse_error(parser, ERR_ATTR_UNQUOTED_EQUALS);
      // Fall through.
    default:
      tokenizer_set_state(parser, ATTR_VALUE_UNQUOTED);
      append_char_to_tag_buffer(parser, c, true);
      return NEXT_CHAR;
  }
}

static StateResult handle_attr_value_double_quoted_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '"':
      tokenizer_set_state(parser, AFTER_ATTR_VALUE_QUOTED);
      return NEXT_CHAR;
    case '&':
      tokenizer->_tag_state._attr_value_state = tokenizer->_state;
      tokenizer_set_state(parser, CHAR_REF_IN_ATTR_VALUE);
      tokenizer->_reconsume_current_input = true;
      return NEXT_CHAR;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      append_char_to_tag_buffer(parser, kUtf8ReplacementChar, false);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_ATTR_DOUBLE_QUOTED_EOF);
      tokenizer_set_state(parser, DATA);
      abandon_current_tag(parser);
      tokenizer->_reconsume_current_input = true;
      return NEXT_CHAR;
    default:
      append_char_to_tag_buffer(parser, c, false);
      return NEXT_CHAR;
  }
}

static StateResult handle_attr_value_single_quoted_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '\'':
      tokenizer_set_state(parser, AFTER_ATTR_VALUE_QUOTED);
      return NEXT_CHAR;
    case '&':
      tokenizer->_tag_state._attr_value_state = tokenizer->_state;
      tokenizer_set_state(parser, CHAR_REF_IN_ATTR_VALUE);
      tokenizer->_reconsume_current_input = true;
      return NEXT_CHAR;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      append_char_to_tag_buffer(parser, kUtf8ReplacementChar, false);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_ATTR_SINGLE_QUOTED_EOF);
      tokenizer_set_state(parser, DATA);
      abandon_current_tag(parser);
      tokenizer->_reconsume_current_input = true;
      return NEXT_CHAR;
    default:
      append_char_to_tag_buffer(parser, c, false);
      return NEXT_CHAR;
  }
}

static StateResult handle_attr_value_unquoted_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      tokenizer_set_state(parser, BEFORE_ATTR_NAME);
      finish_attribute_value(parser);
      return NEXT_CHAR;
    case '&':
      tokenizer->_tag_state._attr_value_state = tokenizer->_state;
      tokenizer_set_state(parser, CHAR_REF_IN_ATTR_VALUE);
      tokenizer->_reconsume_current_input = true;
      return NEXT_CHAR;
    case '>':
      tokenizer_set_state(parser, DATA);
      finish_attribute_value(parser);
      return emit_current_tag(parser, output);
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      append_char_to_tag_buffer(parser, kUtf8ReplacementChar, true);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_ATTR_UNQUOTED_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_reconsume_current_input = true;
      abandon_current_tag(parser);
      return NEXT_CHAR;
    case '<':
    case '=':
    case '"':
    case '\'':
    case '`':
      add_parse_error(parser, ERR_ATTR_UNQUOTED_EQUALS);
      // Fall through.
    default:
      append_char_to_tag_buffer(parser, c, true);
      return NEXT_CHAR;
  }
}

static StateResult handle_char_ref_in_attr_value_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  OneOrTwoCodepoints char_ref;
  int allowed_char;
  bool is_unquoted = false;
  switch (tokenizer->_tag_state._attr_value_state) {
    case ATTR_VALUE_DOUBLE_QUOTED:
      allowed_char = '"';
      break;
    case ATTR_VALUE_SINGLE_QUOTED:
      allowed_char = '\'';
      break;
    case ATTR_VALUE_UNQUOTED:
      allowed_char = '>';
      is_unquoted = true;
      break;
    default:
      allowed_char = ' ';
      assert(0);
  }

  consume_char_ref(parser, &tokenizer->_input, allowed_char, true, &char_ref);
  if (char_ref.first != kNoChar) {
    tokenizer->_reconsume_current_input = true;
    append_char_to_tag_buffer(parser, char_ref.first, is_unquoted);
    if (char_ref.second != kNoChar) {
      append_char_to_tag_buffer(parser, char_ref.second, is_unquoted);
    }
  } else {
    append_char_to_tag_buffer(parser, '&', is_unquoted);
  }
  tokenizer_set_state(parser, tokenizer->_tag_state._attr_value_state);
  return NEXT_CHAR;
}

static StateResult handle_after_attr_value_quoted_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  finish_attribute_value(parser);
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      tokenizer_set_state(parser, BEFORE_ATTR_NAME);
      return NEXT_CHAR;
    case '/':
      tokenizer_set_state(parser, SELF_CLOSING_START_TAG);
      return NEXT_CHAR;
    case '>':
      tokenizer_set_state(parser, DATA);
      return emit_current_tag(parser, output);
    case -1:
      add_parse_error(parser, ERR_ATTR_AFTER_EOF);
      tokenizer_set_state(parser, DATA);
      abandon_current_tag(parser);
      tokenizer->_reconsume_current_input = true;
      return NEXT_CHAR;
    default:
      add_parse_error(parser, ERR_ATTR_AFTER_INVALID);
      tokenizer_set_state(parser, BEFORE_ATTR_NAME);
      tokenizer->_reconsume_current_input = true;
      return NEXT_CHAR;
  }
}

static StateResult handle_self_closing_start_tag_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '>':
      tokenizer_set_state(parser, DATA);
      tokenizer->_tag_state._is_self_closing = true;
      return emit_current_tag(parser, output);
    case -1:
      add_parse_error(parser, ERR_SOLIDUS_EOF);
      tokenizer_set_state(parser, DATA);
      abandon_current_tag(parser);
      return NEXT_CHAR;
    default:
      add_parse_error(parser, ERR_SOLIDUS_INVALID);
      tokenizer_set_state(parser, BEFORE_ATTR_NAME);
      tokenizer->_reconsume_current_input = true;
      return NEXT_CHAR;
  }
}

static StateResult handle_bogus_comment_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  while (c != '>' && c != -1) {
    if (c == '\0') {
      c = 0xFFFD;
    }
    append_char_to_temporary_buffer(parser, c);
    utf8iterator_next(&tokenizer->_input);
    c = utf8iterator_current(&tokenizer->_input);
  }
  tokenizer_set_state(parser, DATA);
  return emit_comment(parser, output);
}

static StateResult handle_markup_declaration_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  if (utf8iterator_maybe_consume_match(&tokenizer->_input, "--", sizeof("--") - 1, true)) {
    tokenizer_set_state(parser, COMMENT_START);
    tokenizer->_reconsume_current_input = true;
  } else if (utf8iterator_maybe_consume_match(&tokenizer->_input, "DOCTYPE", sizeof("DOCTYPE") - 1, false)) {
    tokenizer_set_state(parser, DOCTYPE);
    tokenizer->_reconsume_current_input = true;
    tokenizer->_doc_type_state.name = copy_stringz(parser, "");
    tokenizer->_doc_type_state.public_identifier = copy_stringz(parser, "");
    tokenizer->_doc_type_state.system_identifier = copy_stringz(parser, "");
  } else if (tokenizer->_is_current_node_foreign && utf8iterator_maybe_consume_match(&tokenizer->_input, "[CDATA[", sizeof("[CDATA[") - 1, true)) {
    tokenizer_set_state(parser, CDATA);
    tokenizer->_reconsume_current_input = true;
  } else {
    add_parse_error(parser, ERR_DASHES_OR_DOCTYPE);
    tokenizer_set_state(parser, BOGUS_COMMENT);
    tokenizer->_reconsume_current_input = true;
    clear_temporary_buffer(parser);
  }
  return NEXT_CHAR;
}

static StateResult handle_comment_start_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '-':
      tokenizer_set_state(parser, COMMENT_START_DASH);
      return NEXT_CHAR;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      tokenizer_set_state(parser, COMMENT);
      append_char_to_temporary_buffer(parser, kUtf8ReplacementChar);
      return NEXT_CHAR;
    case '>':
      add_parse_error(parser, ERR_COMMENT_INVALID);
      tokenizer_set_state(parser, DATA);
      emit_comment(parser, output);
      return RETURN_ERROR;
    case -1:
      add_parse_error(parser, ERR_COMMENT_EOF);
      tokenizer_set_state(parser, DATA);
      emit_comment(parser, output);
      return RETURN_ERROR;
    default:
      tokenizer_set_state(parser, COMMENT);
      append_char_to_temporary_buffer(parser, c);
      return NEXT_CHAR;
  }
}

static StateResult handle_comment_start_dash_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '-':
      tokenizer_set_state(parser, COMMENT_END);
      return NEXT_CHAR;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      tokenizer_set_state(parser, COMMENT);
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, kUtf8ReplacementChar);
      return NEXT_CHAR;
    case '>':
      add_parse_error(parser, ERR_COMMENT_INVALID);
      tokenizer_set_state(parser, DATA);
      emit_comment(parser, output);
      return RETURN_ERROR;
    case -1:
      add_parse_error(parser, ERR_COMMENT_EOF);
      tokenizer_set_state(parser, DATA);
      emit_comment(parser, output);
      return RETURN_ERROR;
    default:
      tokenizer_set_state(parser, COMMENT);
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, c);
      return NEXT_CHAR;
  }
}

static StateResult handle_comment_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '-':
      tokenizer_set_state(parser, COMMENT_END_DASH);
      return NEXT_CHAR;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      append_char_to_temporary_buffer(parser, kUtf8ReplacementChar);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_COMMENT_EOF);
      tokenizer_set_state(parser, DATA);
      emit_comment(parser, output);
      return RETURN_ERROR;
    default:
      append_char_to_temporary_buffer(parser, c);
      return NEXT_CHAR;
  }
}

static StateResult handle_comment_end_dash_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '-':
      tokenizer_set_state(parser, COMMENT_END);
      return NEXT_CHAR;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      tokenizer_set_state(parser, COMMENT);
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, kUtf8ReplacementChar);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_COMMENT_EOF);
      tokenizer_set_state(parser, DATA);
      emit_comment(parser, output);
      return RETURN_ERROR;
    default:
      tokenizer_set_state(parser, COMMENT);
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, c);
      return NEXT_CHAR;
  }
}

static StateResult handle_comment_end_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '>':
      tokenizer_set_state(parser, DATA);
      return emit_comment(parser, output);
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      tokenizer_set_state(parser, COMMENT);
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, kUtf8ReplacementChar);
      return NEXT_CHAR;
    case '!':
      add_parse_error(parser, ERR_COMMENT_BANG_AFTER_DOUBLE_DASH);
      tokenizer_set_state(parser, COMMENT_END_BANG);
      return NEXT_CHAR;
    case '-':
      add_parse_error(parser, ERR_COMMENT_DASH_AFTER_DOUBLE_DASH);
      append_char_to_temporary_buffer(parser, '-');
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_UTF8_NULL);
      tokenizer_set_state(parser, DATA);
      emit_comment(parser, output);
      return RETURN_ERROR;
    default:
      add_parse_error(parser, ERR_COMMENT_INVALID);
      tokenizer_set_state(parser, COMMENT);
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, c);
      return NEXT_CHAR;
  }
}

static StateResult handle_comment_end_bang_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '-':
      tokenizer_set_state(parser, COMMENT_END_DASH);
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, '!');
      return NEXT_CHAR;
    case '>':
      tokenizer_set_state(parser, DATA);
      return emit_comment(parser, output);
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      tokenizer_set_state(parser, COMMENT);
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, '!');
      append_char_to_temporary_buffer(parser, kUtf8ReplacementChar);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_COMMENT_END_BANG_EOF);
      tokenizer_set_state(parser, DATA);
      emit_comment(parser, output);
      return RETURN_ERROR;
    default:
      tokenizer_set_state(parser, COMMENT);
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, '-');
      append_char_to_temporary_buffer(parser, '!');
      append_char_to_temporary_buffer(parser, c);
      return NEXT_CHAR;
  }
}

static StateResult handle_doctype_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  assert(!tokenizer->_temporary_buffer.length);
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      tokenizer_set_state(parser, BEFORE_DOCTYPE_NAME);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      add_parse_error(parser, ERR_DOCTYPE_SPACE);
      tokenizer_set_state(parser, BEFORE_DOCTYPE_NAME);
      tokenizer->_reconsume_current_input = true;
      tokenizer->_doc_type_state.force_quirks = true;
      return NEXT_CHAR;
  }
}

static StateResult handle_before_doctype_name_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      return NEXT_CHAR;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      tokenizer_set_state(parser, DOCTYPE_NAME);
      tokenizer->_doc_type_state.force_quirks = true;
      append_char_to_temporary_buffer(parser, kUtf8ReplacementChar);
      return NEXT_CHAR;
    case '>':
      add_parse_error(parser, ERR_DOCTYPE_RIGHT_BRACKET);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      tokenizer_set_state(parser, DOCTYPE_NAME);
      tokenizer->_doc_type_state.force_quirks = false;
      append_char_to_temporary_buffer(parser, ensure_lowercase(c));
      return NEXT_CHAR;
  }
}

static StateResult handle_doctype_name_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      tokenizer_set_state(parser, AFTER_DOCTYPE_NAME);
      parser_deallocate(parser, (void*) tokenizer->_doc_type_state.name);
      finish_temporary_buffer(parser, &tokenizer->_doc_type_state.name);
      return NEXT_CHAR;
    case '>':
      tokenizer_set_state(parser, DATA);
      parser_deallocate(parser, (void*) tokenizer->_doc_type_state.name);
      finish_temporary_buffer(parser, &tokenizer->_doc_type_state.name);
      emit_doctype(parser, output);
      return RETURN_SUCCESS;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      append_char_to_temporary_buffer(parser, kUtf8ReplacementChar);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      parser_deallocate(parser, (void*) tokenizer->_doc_type_state.name);
      finish_temporary_buffer(parser, &tokenizer->_doc_type_state.name);
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      tokenizer_set_state(parser, DOCTYPE_NAME);
      tokenizer->_doc_type_state.force_quirks = false;
      append_char_to_temporary_buffer(parser, ensure_lowercase(c));
      return NEXT_CHAR;
  }
}

static StateResult handle_after_doctype_name_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      return NEXT_CHAR;
    case '>':
      tokenizer_set_state(parser, DATA);
      emit_doctype(parser, output);
      return RETURN_SUCCESS;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      if (utf8iterator_maybe_consume_match(&tokenizer->_input, "PUBLIC", sizeof("PUBLIC") - 1, false)) {
        tokenizer_set_state(parser, AFTER_DOCTYPE_PUBLIC_KEYWORD);
        tokenizer->_reconsume_current_input = true;
      } else if (utf8iterator_maybe_consume_match(&tokenizer->_input, "SYSTEM", sizeof("SYSTEM") - 1, false)) {
        tokenizer_set_state(parser, AFTER_DOCTYPE_SYSTEM_KEYWORD);
        tokenizer->_reconsume_current_input = true;
      } else {
        add_parse_error(parser, ERR_DOCTYPE_SPACE_OR_RIGHT_BRACKET);
        tokenizer_set_state(parser, BOGUS_DOCTYPE);
        tokenizer->_doc_type_state.force_quirks = true;
      }
      return NEXT_CHAR;
  }
}

static StateResult handle_after_doctype_public_keyword_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      tokenizer_set_state(parser, BEFORE_DOCTYPE_PUBLIC_ID);
      return NEXT_CHAR;
    case '"':
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      assert(temporary_buffer_equals(parser, ""));
      tokenizer_set_state(parser, DOCTYPE_PUBLIC_ID_DOUBLE_QUOTED);
      return NEXT_CHAR;
    case '\'':
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      assert(temporary_buffer_equals(parser, ""));
      tokenizer_set_state(parser, DOCTYPE_PUBLIC_ID_SINGLE_QUOTED);
      return NEXT_CHAR;
    case '>':
      add_parse_error(parser, ERR_DOCTYPE_RIGHT_BRACKET);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      tokenizer_set_state(parser, BOGUS_DOCTYPE);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
  }
}

static StateResult handle_before_doctype_public_id_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      return NEXT_CHAR;
    case '"':
      assert(temporary_buffer_equals(parser, ""));
      tokenizer_set_state(parser, DOCTYPE_PUBLIC_ID_DOUBLE_QUOTED);
      return NEXT_CHAR;
    case '\'':
      assert(temporary_buffer_equals(parser, ""));
      tokenizer_set_state(parser, DOCTYPE_PUBLIC_ID_SINGLE_QUOTED);
      return NEXT_CHAR;
    case '>':
      add_parse_error(parser, ERR_DOCTYPE_END);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      tokenizer_set_state(parser, BOGUS_DOCTYPE);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
  }
}

static StateResult handle_doctype_public_id_double_quoted_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '"':
      tokenizer_set_state(parser, AFTER_DOCTYPE_PUBLIC_ID);
      finish_doctype_public_id(parser);
      return NEXT_CHAR;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      append_char_to_temporary_buffer(parser, kUtf8ReplacementChar);
      return NEXT_CHAR;
    case '>':
      add_parse_error(parser, ERR_DOCTYPE_END);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      finish_doctype_public_id(parser);
      emit_doctype(parser, output);
      return RETURN_ERROR;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      finish_doctype_public_id(parser);
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      append_char_to_temporary_buffer(parser, c);
      return NEXT_CHAR;
  }
}

static StateResult handle_doctype_public_id_single_quoted_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '\'':
      tokenizer_set_state(parser, AFTER_DOCTYPE_PUBLIC_ID);
      finish_doctype_public_id(parser);
      return NEXT_CHAR;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      append_char_to_temporary_buffer(parser, kUtf8ReplacementChar);
      return NEXT_CHAR;
    case '>':
      add_parse_error(parser, ERR_DOCTYPE_END);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      finish_doctype_public_id(parser);
      emit_doctype(parser, output);
      return RETURN_ERROR;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      finish_doctype_public_id(parser);
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      append_char_to_temporary_buffer(parser, c);
      return NEXT_CHAR;
  }
}

static StateResult handle_after_doctype_public_id_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      tokenizer_set_state(parser, BETWEEN_DOCTYPE_PUBLIC_SYSTEM_ID);
      return NEXT_CHAR;
    case '>':
      tokenizer_set_state(parser, DATA);
      emit_doctype(parser, output);
      return RETURN_SUCCESS;
    case '"':
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      assert(temporary_buffer_equals(parser, ""));
      tokenizer_set_state(parser, DOCTYPE_SYSTEM_ID_DOUBLE_QUOTED);
      return NEXT_CHAR;
    case '\'':
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      assert(temporary_buffer_equals(parser, ""));
      tokenizer_set_state(parser, DOCTYPE_SYSTEM_ID_SINGLE_QUOTED);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_reconsume_current_input = true;
      tokenizer->_doc_type_state.force_quirks = true;
      return NEXT_CHAR;
    default:
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      tokenizer_set_state(parser, BOGUS_DOCTYPE);
      tokenizer->_doc_type_state.force_quirks = true;
      return NEXT_CHAR;
  }
}

static StateResult handle_between_doctype_public_system_id_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      return NEXT_CHAR;
    case '>':
      tokenizer_set_state(parser, DATA);
      emit_doctype(parser, output);
      return RETURN_SUCCESS;
    case '"':
      assert(temporary_buffer_equals(parser, ""));
      tokenizer_set_state(parser, DOCTYPE_SYSTEM_ID_DOUBLE_QUOTED);
      return NEXT_CHAR;
    case '\'':
      assert(temporary_buffer_equals(parser, ""));
      tokenizer_set_state(parser, DOCTYPE_SYSTEM_ID_SINGLE_QUOTED);
      return NEXT_CHAR;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      tokenizer_set_state(parser, BOGUS_DOCTYPE);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
  }
}

static StateResult handle_after_doctype_system_keyword_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      tokenizer_set_state(parser, BEFORE_DOCTYPE_SYSTEM_ID);
      return NEXT_CHAR;
    case '"':
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      assert(temporary_buffer_equals(parser, ""));
      tokenizer_set_state(parser, DOCTYPE_SYSTEM_ID_DOUBLE_QUOTED);
      return NEXT_CHAR;
    case '\'':
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      assert(temporary_buffer_equals(parser, ""));
      tokenizer_set_state(parser, DOCTYPE_SYSTEM_ID_SINGLE_QUOTED);
      return NEXT_CHAR;
    case '>':
      add_parse_error(parser, ERR_DOCTYPE_END);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      tokenizer_set_state(parser, BOGUS_DOCTYPE);
      tokenizer->_doc_type_state.force_quirks = true;
      return NEXT_CHAR;
  }
}

static StateResult handle_before_doctype_system_id_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      return NEXT_CHAR;
    case '"':
      assert(temporary_buffer_equals(parser, ""));
      tokenizer_set_state(parser, DOCTYPE_SYSTEM_ID_DOUBLE_QUOTED);
      return NEXT_CHAR;
    case '\'':
      assert(temporary_buffer_equals(parser, ""));
      tokenizer_set_state(parser, DOCTYPE_SYSTEM_ID_SINGLE_QUOTED);
      return NEXT_CHAR;
    case '>':
      add_parse_error(parser, ERR_DOCTYPE_END);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      tokenizer_set_state(parser, BOGUS_DOCTYPE);
      tokenizer->_doc_type_state.force_quirks = true;
      return NEXT_CHAR;
  }
}

static StateResult handle_doctype_system_id_double_quoted_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '"':
      tokenizer_set_state(parser, AFTER_DOCTYPE_SYSTEM_ID);
      finish_doctype_system_id(parser);
      return NEXT_CHAR;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      append_char_to_temporary_buffer(parser, kUtf8ReplacementChar);
      return NEXT_CHAR;
    case '>':
      add_parse_error(parser, ERR_DOCTYPE_END);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      finish_doctype_system_id(parser);
      emit_doctype(parser, output);
      return RETURN_ERROR;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      finish_doctype_system_id(parser);
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      append_char_to_temporary_buffer(parser, c);
      return NEXT_CHAR;
  }
}

static StateResult handle_doctype_system_id_single_quoted_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output){
  switch (c) {
    case '\'':
      tokenizer_set_state(parser, AFTER_DOCTYPE_SYSTEM_ID);
      finish_doctype_system_id(parser);
      return NEXT_CHAR;
    case '\0':
      add_parse_error(parser, ERR_UTF8_NULL);
      append_char_to_temporary_buffer(parser, kUtf8ReplacementChar);
      return NEXT_CHAR;
    case '>':
      add_parse_error(parser, ERR_DOCTYPE_END);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      finish_doctype_system_id(parser);
      emit_doctype(parser, output);
      return RETURN_ERROR;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      finish_doctype_system_id(parser);
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      append_char_to_temporary_buffer(parser, c);
      return NEXT_CHAR;
  }
}

static StateResult handle_after_doctype_system_id_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  switch (c) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
      return NEXT_CHAR;
    case '>':
      tokenizer_set_state(parser, DATA);
      emit_doctype(parser, output);
      return RETURN_SUCCESS;
    case -1:
      add_parse_error(parser, ERR_DOCTYPE_EOF);
      tokenizer_set_state(parser, DATA);
      tokenizer->_doc_type_state.force_quirks = true;
      emit_doctype(parser, output);
      return RETURN_ERROR;
    default:
      add_parse_error(parser, ERR_DOCTYPE_INVALID);
      tokenizer_set_state(parser, BOGUS_DOCTYPE);
      return NEXT_CHAR;
  }
}

static StateResult handle_bogus_doctype_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  if (c == '>' || c == -1) {
    tokenizer_set_state(parser, DATA);
    emit_doctype(parser, output);
    return RETURN_ERROR;
  }
  return NEXT_CHAR;
}

static StateResult handle_cdata_state(Parser* parser, TokenizerState* tokenizer, int c, Token* output) {
  if (c == -1 || utf8iterator_maybe_consume_match(&tokenizer->_input, "]]>", sizeof("]]>") - 1, true)) {
    tokenizer->_reconsume_current_input = true;
    reset_token_start_point(tokenizer);
    tokenizer_set_state(parser, DATA);
    return NEXT_CHAR;
  } else {
    return emit_current_char(parser, output);
  }
}

typedef StateResult (*LexerStateFunction)(Parser*, TokenizerState*, int, Token*);

static LexerStateFunction dispatch_table[] = {
  handle_data_state,
  handle_char_ref_in_data_state,
  handle_rcdata_state,
  handle_char_ref_in_rcdata_state,
  handle_rawtext_state,
  handle_script_state,
  handle_plaintext_state,
  handle_tag_open_state,
  handle_end_tag_open_state,
  handle_tag_name_state,
  handle_rcdata_lt_state,
  handle_rcdata_end_tag_open_state,
  handle_rcdata_end_tag_name_state,
  handle_rawtext_lt_state,
  handle_rawtext_end_tag_open_state,
  handle_rawtext_end_tag_name_state,
  handle_script_lt_state,
  handle_script_end_tag_open_state,
  handle_script_end_tag_name_state,
  handle_script_escaped_start_state,
  handle_script_escaped_start_dash_state,
  handle_script_escaped_state,
  handle_script_escaped_dash_state,
  handle_script_escaped_dash_dash_state,
  handle_script_escaped_lt_state,
  handle_script_escaped_end_tag_open_state,
  handle_script_escaped_end_tag_name_state,
  handle_script_double_escaped_start_state,
  handle_script_double_escaped_state,
  handle_script_double_escaped_dash_state,
  handle_script_double_escaped_dash_dash_state,
  handle_script_double_escaped_lt_state,
  handle_script_double_escaped_end_state,
  handle_before_attr_name_state,
  handle_attr_name_state,
  handle_after_attr_name_state,
  handle_before_attr_value_state,
  handle_attr_value_double_quoted_state,
  handle_attr_value_single_quoted_state,
  handle_attr_value_unquoted_state,
  handle_char_ref_in_attr_value_state,
  handle_after_attr_value_quoted_state,
  handle_self_closing_start_tag_state,
  handle_bogus_comment_state,
  handle_markup_declaration_state,
  handle_comment_start_state,
  handle_comment_start_dash_state,
  handle_comment_state,
  handle_comment_end_dash_state,
  handle_comment_end_state,
  handle_comment_end_bang_state,
  handle_doctype_state,
  handle_before_doctype_name_state,
  handle_doctype_name_state,
  handle_after_doctype_name_state,
  handle_after_doctype_public_keyword_state,
  handle_before_doctype_public_id_state,
  handle_doctype_public_id_double_quoted_state,
  handle_doctype_public_id_single_quoted_state,
  handle_after_doctype_public_id_state,
  handle_between_doctype_public_system_id_state,
  handle_after_doctype_system_keyword_state,
  handle_before_doctype_system_id_state,
  handle_doctype_system_id_double_quoted_state,
  handle_doctype_system_id_single_quoted_state,
  handle_after_doctype_system_id_state,
  handle_bogus_doctype_state,
  handle_cdata_state
};

bool lex(Parser* parser, Token* output) {
  TokenizerState* tokenizer = parser->_tokenizer_state;

  if (tokenizer->_buffered_emit_char != kNoChar) {
    tokenizer->_reconsume_current_input = true;
    emit_char(parser, tokenizer->_buffered_emit_char, output);
    tokenizer->_reconsume_current_input = false;
    tokenizer->_buffered_emit_char = kNoChar;
    return true;
  }

  if (maybe_emit_from_temporary_buffer(parser, output)) {
    return true;
  }

  while (1) {
    assert(!tokenizer->_temporary_buffer_emit);
    assert(tokenizer->_buffered_emit_char == kNoChar);
    int c = utf8iterator_current(&tokenizer->_input);
    debug("Lexing character '%c' in state %d.\n", c, tokenizer->_state);
    StateResult result = dispatch_table[tokenizer->_state](parser, tokenizer, c, output);
    bool should_advance = !tokenizer->_reconsume_current_input;
    tokenizer->_reconsume_current_input = false;

    if (result == RETURN_SUCCESS) {
      return true;
    } else if(result == RETURN_ERROR) {
      return false;
    }

    if (should_advance) {
      utf8iterator_next(&tokenizer->_input);
    }
  }
}

void token_destroy(Parser* parser, Token* token) {
  if (!token) return;

  switch (token->type) {
    case TOKEN_DOCTYPE:
      parser_deallocate(parser, (void*) token->v.doc_type.name);
      parser_deallocate(parser, (void*) token->v.doc_type.public_identifier);
      parser_deallocate(parser, (void*) token->v.doc_type.system_identifier);
      return;
    case TOKEN_START_TAG:
      for (int i = 0; i < token->v.start_tag.attributes.length; ++i) {
        Attribute* attr = token->v.start_tag.attributes.data[i];
        if (attr) {
          // May have been nulled out if this token was merged with another.
          destroy_attribute(parser, attr);
        }
      }
      parser_deallocate(
          parser, (void*) token->v.start_tag.attributes.data);
      return;
    case TOKEN_COMMENT:
      parser_deallocate(parser, (void*) token->v.text);
      return;
    default:
      return;
  }
}
