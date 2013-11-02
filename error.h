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
