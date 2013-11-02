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
#ifndef TOKENIZER_STATES_H_
#define TOKENIZER_STATES_H_

typedef enum {
    DATA,
    CHAR_REF_IN_DATA,
    RCDATA,
    CHAR_REF_IN_RCDATA,
    RAWTEXT,
    SCRIPT,
    PLAINTEXT,
    TAG_OPEN,
    END_TAG_OPEN,
    TAG_NAME,
    RCDATA_LT,
    RCDATA_END_TAG_OPEN,
    RCDATA_END_TAG_NAME,
    RAWTEXT_LT,
    RAWTEXT_END_TAG_OPEN,
    RAWTEXT_END_TAG_NAME,
    SCRIPT_LT,
    SCRIPT_END_TAG_OPEN,
    SCRIPT_END_TAG_NAME,
    SCRIPT_ESCAPED_START,
    SCRIPT_ESCAPED_START_DASH,
    SCRIPT_ESCAPED,
    SCRIPT_ESCAPED_DASH,
    SCRIPT_ESCAPED_DASH_DASH,
    SCRIPT_ESCAPED_LT,
    SCRIPT_ESCAPED_END_TAG_OPEN,
    SCRIPT_ESCAPED_END_TAG_NAME,
    SCRIPT_DOUBLE_ESCAPED_START,
    SCRIPT_DOUBLE_ESCAPED,
    SCRIPT_DOUBLE_ESCAPED_DASH,
    SCRIPT_DOUBLE_ESCAPED_DASH_DASH,
    SCRIPT_DOUBLE_ESCAPED_LT,
    SCRIPT_DOUBLE_ESCAPED_END,
    BEFORE_ATTR_NAME,
    ATTR_NAME,
    AFTER_ATTR_NAME,
    BEFORE_ATTR_VALUE,
    ATTR_VALUE_DOUBLE_QUOTED,
    ATTR_VALUE_SINGLE_QUOTED,
    ATTR_VALUE_UNQUOTED,
    CHAR_REF_IN_ATTR_VALUE,
    AFTER_ATTR_VALUE_QUOTED,
    SELF_CLOSING_START_TAG,
    BOGUS_COMMENT,
    MARKUP_DECLARATION,
    COMMENT_START,
    COMMENT_START_DASH,
    COMMENT,
    COMMENT_END_DASH,
    COMMENT_END,
    COMMENT_END_BANG,
    DOCTYPE,
    BEFORE_DOCTYPE_NAME,
    DOCTYPE_NAME,
    AFTER_DOCTYPE_NAME,
    AFTER_DOCTYPE_PUBLIC_KEYWORD,
    BEFORE_DOCTYPE_PUBLIC_ID,
    DOCTYPE_PUBLIC_ID_DOUBLE_QUOTED,
    DOCTYPE_PUBLIC_ID_SINGLE_QUOTED,
    AFTER_DOCTYPE_PUBLIC_ID,
    BETWEEN_DOCTYPE_PUBLIC_SYSTEM_ID,
    AFTER_DOCTYPE_SYSTEM_KEYWORD,
    BEFORE_DOCTYPE_SYSTEM_ID,
    DOCTYPE_SYSTEM_ID_DOUBLE_QUOTED,
    DOCTYPE_SYSTEM_ID_SINGLE_QUOTED,
    AFTER_DOCTYPE_SYSTEM_ID,
    BOGUS_DOCTYPE,
    CDATA
}TokenizerEnum;

#endif // TOKENIZER_STATES_H_
