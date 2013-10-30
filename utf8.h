#ifndef UTF8_H_
#define UTF8_H_

#include <stdbool.h>
#include <stddef.h>
#include "html.h"

#ifdef __cplusplus
extern "C"{
#endif

struct InternalError;
struct InternalParser;

extern const int kUtf8ReplacementChar;

typedef struct InternalUtf8Iterator{
    const char* _start;
    const char* _mark;
    const char* _end;
    int _current;
    int _width;
    SrcPosition _pos;
    SrcPosition _mark_pos;
    struct InternalParser* _parser;
}Utf8Iterator;

bool utf8_is_invalid_code_point(int c);
void utf8iterator_init(struct InternalParser* parser, const char* source, size_t source_length, Utf8Iterator* iter);
void utf8iterator_next(Utf8Iterator* iter);
int utf8iterator_current(const Utf8Iterator* iter);
void utf8iterator_get_position(const Utf8Iterator* iter, SrcPosition* output);
const char* utf8iterator_get_char_pointer(const Utf8Iterator* iter);
bool utf8iterator_maybe_consume_match(Utf8Iterator* iter, const char* prefix, size_t length, bool case_sensitive);
void utf8iterator_mark(Utf8Iterator* iter);
void utf8iterator_reset(Utf8Iterator* iter);
void utf8iterator_fill_error_at_mark(Utf8Iterator* iter, struct InternalError* error);

#ifdef __cplusplus
}
#endif

#endif // UTF8_H_
