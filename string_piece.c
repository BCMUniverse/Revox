#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "string_piece.h"
#include "util.h"

struct InternalParser;

const StringPiece kEmptyString = { NULL, 0 };

bool string_equals(const StringPiece* str1, const StringPiece* str2) {
  return str1->length == str2->length && !memcmp(str1->data, str2->data, str1->length);
}

bool string_equals_ignore_case(const StringPiece* str1, const StringPiece* str2) {
  return str1->length == str2->length && !strncasecmp(str1->data, str2->data, str1->length);
}

void string_copy(struct InternalParser* parser, StringPiece* dest, const StringPiece* source) {
  dest->length = source->length;
  char* buffer = parser_allocate(parser, source->length);
  memcpy(buffer, source->data, source->length);
  dest->data = buffer;
}
