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
#ifndef INSERTION_MODE_H_
#define INSERTION_MODE_H_

#ifdef __cplusplus
extern "C"{
#endif

typedef enum {
    INSERTION_MODE_INITIAL,
    INSERTION_MODE_BEFORE_HTML,
    INSERTION_MODE_BEFORE_HEAD,
    INSERTION_MODE_IN_HEAD,
    INSERTION_MODE_IN_HEAD_NOSCRIPT,
    INSERTION_MODE_AFTER_HEAD,
    INSERTION_MODE_IN_BODY,
    INSERTION_MODE_TEXT,
    INSERTION_MODE_IN_TABLE,
    INSERTION_MODE_IN_TABLE_TEXT,
    INSERTION_MODE_IN_CAPTION,
    INSERTION_MODE_IN_COLUMN_GROUP,
    INSERTION_MODE_IN_TABLE_BODY,
    INSERTION_MODE_IN_ROW,
    INSERTION_MODE_IN_CELL,
    INSERTION_MODE_IN_SELECT,
    INSERTION_MODE_IN_SELECT_IN_TABLE,
    INSERTION_MODE_AFTER_BODY,
    INSERTION_MODE_IN_FRAMESET,
    INSERTION_MODE_AFTER_FRAMESET,
    INSERTION_MODE_AFTER_AFTER_BODY,
    INSERTION_MODE_AFTER_AFTER_FRAMESET
}InsertionMode;

#ifdef __cplusplus
}
#endif

#endif // INSERTION_MODE_H_
