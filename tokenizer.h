#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <stdbool.h>
#include <stddef.h>

#include "html.h"
#include "token_type.h"
#include "tokenizer_states.h"

#ifdef __cplusplus
extern "C" {
#endif

struct InternalParser;

typedef struct InternalTokenDocType {
  const char* name;
  const char* public_identifier;
  const char* system_identifier;
  bool force_quirks;
  bool has_public_identifier;
  bool has_system_identifier;
} TokenDocType;

typedef struct InternalTokenStartTag {
  HTMLTag tag;
  Vector /* Attribute */ attributes;
  bool is_self_closing;
}TokenStartTag;

typedef struct InternalToken {
  GTokenType type;
  SrcPosition position;
  StringPiece original_text;
  union {
    TokenDocType doc_type;
    TokenStartTag start_tag;
    HTMLTag end_tag;
    const char* text;
    int character;
  } v;
}Token;

void tokenizer_state_init(struct InternalParser* parser, const char* text, size_t text_length);
void tokenizer_state_destroy(struct InternalParser* parser);
void tokenizer_set_state(struct InternalParser* parser, TokenizerEnum state);
void tokenizer_set_is_current_node_foreign(struct InternalParser* parser, bool is_foreign);
bool lex(struct InternalParser* parser, Token* output);
void token_destroy(struct InternalParser* parser, Token* token);

#ifdef __cplusplus
}
#endif

#endif  // TOKENIZER_H_
