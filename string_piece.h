#ifndef STRING_PIECE_H_
#define STRING_PIECE_H_

#include "html.h"

#ifdef __cplusplus
extern "C"{
#endif

struct InternalParser;

void string_copy(struct InternalParser* parser, StringPiece* dest, const StringPiece* source);

#ifdef __cplusplus
}
#endif

#endif // STRING_PIECE_H_
