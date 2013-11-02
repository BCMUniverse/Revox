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
#include <assert.h>
#include <ctype.h>
#include <strings.h>
#include "html.h"

// TODO(jdtang): Investigate whether there're efficiency benefits to putting the
// most common tag names first, or to putting them in alphabetical order and
// using a binary search.
const char* kTagNames[] = {"html", "head", "title", "body", "p", "", ""};
const char* normalized_tagname(HTMLTag tag){
    assert(tag <= TAG_LAST);
    return kTagNames[tag];
}

// TODO(jdtang): Add test for this.
void tag_from_original_text(StringPiece* text){
    if(text->data == NULL){
        return;
    }
    assert(text->length >= 2);
    assert(text->data[0] == '<');
    assert(text->data[text->length-1] == '>');
    if(text->data[1] == '/'){
        //Tag Final
        assert(text->length >= 3);
        text->data += 2; //Segue em frente de </
        text->length -= 2;
    }
    else{
        //Tag Inicial
        text->data += 1;
        text->length -= 2;
        const char* c;
        for(c = text->data;c != text->data + text->length; c++){
            if(isspace(*c)||*c == '/'){
                text->length = c-text->data;
                break;
            }
        }
    }
}

HTMLTag tag_enum(const char* tagname){
    int i;
    for(i=0; i < TAG_LAST; ++i){
        if(strcasecmp(tagname, kTagNames[i]) == 0){
            return i;
        }
    }
    return TAG_UNKNOWN;
}
