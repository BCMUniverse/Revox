#ifndef ERROR_H_
#define ERROR_H_

#include <stdint.h>
#include "html.h"
#include "insertion_mode.h"
#include "string_buffer.h"
#include "token_type.h"

#ifdef __cplusplus
extern "C"{
#endif

struct InternalParser;

typedef enum {
    ERR_UTF8_INVALID,
    ERR_UTF8_TRUNCATED,
    ERR_UTF8_NULL,
    ERR_NUMERIC_CHAR_REF_NO_DIGITS,
    ERR_NUMERIC_CHAR_REF_WITHOUT_SEMICOLON,
    ERR_NUMERIC_CHAR_REF_INVALID,
    ERR_NAMED_CHAR_REF_WITHOUT_SEMICOLON,
    ERR_NAMED_CHAR_REF_INVALID,
    ERR_TAG_STARTS_WITH_QUESTION,
    ERR_TAG_EOF,
    ERR_TAG_INVALID,
    ERR_CLOSE_TAG_EMPTY,
    ERR_CLOSE_TAG_EOF,
    ERR_CLOSE_TAG_INVALID,
    ERR_SCRIPT_EOF,
    ERR_ATTR_NAME_EOF,
    ERR_ATTR_NAME_INVALID,
    ERR_ATTR_DOUBLE_QUOTED_EOF,
    ERR_ATTR_SINGLE_QUOTED_EOF,
    ERR_ATTR_UNQUOTED_EOF,
    ERR_ATTR_UNQUOTED_RIGHT_BRACKET,
    ERR_ATTR_UNQUOTED_EQUALS,
    ERR_ATTR_AFTER_EOF,
    ERR_ATTR_AFTER_INVALID,
    ERR_DUPLICATE_ATTR,
    ERR_SOLIDUS_EOF,
    ERR_SOLIDUS_INVALID,
    ERR_DASHES_OR_DOCTYPE,
    ERR_COMMENT_EOF,
    ERR_COMMENT_INVALID,
    ERR_COMMENT_BANG_AFTER_DOUBLE_DASH,
    ERR_COMMENT_DASH_AFTER_DOUBLE_DASH,
    ERR_COMMENT_SPACE_AFTER_DOUBLE_DASH,
    ERR_COMMENT_END_BANG_EOF,
    ERR_DOCTYPE_EOF,
    ERR_DOCTYPE_INVALID,
    ERR_DOCTYPE_SPACE,
    ERR_DOCTYPE_RIGHT_BRACKET,
    ERR_DOCTYPE_SPACE_OR_RIGHT_BRACKET,
    ERR_DOCTYPE_END,
    ERR_PARSER,
    ERR_UNACKNOWLEDGED_SELF_CLOSING_TAG,
}ErrorType;

typedef struct InternalDuplicateAttrError{
    const char* name;
    unsigned int original_index;
    unsigned int new_index;
}DuplicateAttrError;

typedef enum{
    ERR_TOKENIZER_DATA,
    ERR_TOKENIZER_CHAR_REF,
    ERR_TOKENIZER_RCDATA,
    ERR_TOKENIZER_RAWTEXT,
    ERR_TOKENIZER_PLAINTEXT,
    ERR_TOKENIZER_SCRIPT,
    ERR_TOKENIZER_TAG,
    ERR_TOKENIZER_SELF_CLOSING_TAG,
    ERR_TOKENIZER_ATTR_NAME,
    ERR_TOKENIZER_ATTR_VALUE,
    ERR_TOKENIZER_MARKUP_DECLARATION,
    ERR_TOKENIZER_COMMENT,
    ERR_TOKENIZER_DOCTYPE,
    ERR_TOKENIZER_CDATA
}TokenizerErrorState;

typedef struct InternalTokenizerError{
    int codepoint;
    TokenizerErrorState state;
}TokenizerError;

typedef struct InternalParserError{
    GTokenType input_type;
    HTMLTag input_tag;
    InsertionMode parser_state;
    Vector /*  HTMLTag */ tag_stack;
}ParserError;

typedef struct InternalError{
    ErrorType type;
    SrcPosition position;
    const char* original_text;
    union{
        uint64_t codepoint;
        TokenizerError tokenizer;
        StringPiece text;
        DuplicateAttrError duplicate_attr;
        struct InternalParserError parser;
    }v;
}Error;

Error* add_error(struct InternalParser* parser);
void init_errors(struct InternalParser* errors);
void destroy_errors(struct InternalParser* errors);
void error_destroy(struct InternalParser* parser, Error* error);
void error_to_string(struct InternalParser* parser, const Error* error, StringBuffer* output);
void caret_diagnostic_to_string(struct InternalParser* parser, const Error* error, const char* source_text, StringBuffer* output);
void print_caret_diagnostic(struct InternalParser* parser, const Error* error, const char* source_text);

#ifdef __cplusplus
}
#endif

#endif // ERROR_H_
