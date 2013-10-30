#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "string_buffer.h"
#include "string_piece.h"
#include "util.h"

struct InternalParser;

static const size_t kDefaultStringBufferSize = 10;

static void maybe_resize_string_buffer(struct InternalParser* parser, size_t additional_chars, StringBuffer* buffer) {
  size_t new_length = buffer->length + additional_chars;
  size_t new_capacity = buffer->capacity;
  while (new_capacity < new_length) {
    new_capacity *= 2;
  }
  if (new_capacity != buffer->capacity) {
    char* new_data = parser_allocate(parser, new_capacity);
    memcpy(new_data, buffer->data, buffer->length);
    parser_deallocate(parser, buffer->data);
    buffer->data = new_data;
    buffer->capacity = new_capacity;
  }
}

void string_buffer_init(struct InternalParser* parser, StringBuffer* output) {
  output->data = parser_allocate(parser, kDefaultStringBufferSize);
  output->length = 0;
  output->capacity = kDefaultStringBufferSize;
}

void string_buffer_reserve(struct InternalParser* parser, size_t min_capacity, StringBuffer* output) {
  maybe_resize_string_buffer(parser, min_capacity - output->length, output);
}

void string_buffer_append_codepoint(struct InternalParser* parser, int c, StringBuffer* output) {
  int num_bytes, prefix;
  if (c <= 0x7f) {
    num_bytes = 0;
    prefix = 0;
  } else if (c <= 0x7ff) {
    num_bytes = 1;
    prefix = 0xc0;
  } else if (c <= 0xffff) {
    num_bytes = 2;
    prefix = 0xe0;
  } else {
    num_bytes = 3;
    prefix = 0xf0;
  }
  maybe_resize_string_buffer(parser, num_bytes + 1, output);
  output->data[output->length++] = prefix | (c >> (num_bytes * 6));
  for (int i = num_bytes - 1; i >= 0; --i) {
    output->data[output->length++] = 0x80 | (0x3f & (c >> (i * 6)));
  }
}

void string_buffer_append_string(struct InternalParser* parser, StringPiece* str, StringBuffer* output) {
  maybe_resize_string_buffer(parser, str->length, output);
  memcpy(output->data + output->length, str->data, str->length);
  output->length += str->length;
}

char* string_buffer_to_string(struct InternalParser* parser, StringBuffer* input) {
  char* buffer = parser_allocate(parser, input->length + 1);
  memcpy(buffer, input->data, input->length);
  buffer[input->length] = '\0';
  return buffer;
}

void string_buffer_destroy(struct InternalParser* parser, StringBuffer* buffer) {
  parser_deallocate(parser, buffer->data);
}
