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
