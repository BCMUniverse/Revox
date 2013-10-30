#ifndef CHAR_REF_H_
#define CHAR_REF_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

struct InternalParser;
struct InternalUtf8Iterator;

extern const int kNoChar;

typedef struct{
    int first;
    int second;
}OneOrTwoCodepoints;

bool consume_char_ref(struct InternalParser* parser, struct InternalUtf8Iterator* input, int additional_allowed_char, bool is_in_attribute, OneOrTwoCodepoints* output);

#ifdef __cplusplus
}
#endif

#endif // CHAR_REF_H_
