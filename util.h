#ifndef UTIL_H_
#define UTIL_H_

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif

struct InternalParser;

char* copy_stringz(struct InternalParser* parser, const char* str);
void* parser_allocate(struct InternalParser* parser, size_t num_bytes);
void parser_deallocate(struct InternalParser* parser, void* ptr);
void debug(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif // UTIL_H_
