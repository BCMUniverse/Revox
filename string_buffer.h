#ifndef STRING_BUFFER_H_
#define STRING_BUFFER_H_

#include <stdbool.h>
#include <stddef.h>
#include "html.h"

#ifdef __cplusplus
extern "C"{
#endif

struct InternalParser;

typedef struct {
    char* data;
    size_t length;
    size_t capacity;
}StringBuffer;

void string_buffer_init(struct InternalParser* parser, StringBuffer* output);
void string_buffer_reserve(struct InternalParser* parser, size_t min_capacity, StringBuffer* output);
void string_buffer_append_codepoint(struct InternalParser* parser, int c, StringBuffer* output);
void string_buffer_append_string(struct InternalParser* parser, StringPiece* str, StringBuffer* output);
char* string_buffer_to_string(struct InternalParser* parser, StringBuffer* output);
void string_buffer_destroy(struct InternalParser* parser, StringBuffer* buffer);

#ifdef __cplusplus
}
#endif

#endif // STRING_BUFFER_H_
