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
