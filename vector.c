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
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "vector.h"
#include "util.h"

struct InternalParser;

const Vector kEmptyVector = { NULL, 0, 0 };

void vector_init(struct InternalParser* parser, size_t initial_capacity, Vector* vtr) {
  vtr->length = 0;
  vtr->capacity = initial_capacity;
  if (initial_capacity > 0) {
    vtr->data = parser_allocate(parser, sizeof(void*) * initial_capacity);
  } else {
    vtr->data = NULL;
  }
}

void vector_destroy(struct InternalParser* parser, Vector* vtr) {
  if (vtr->capacity > 0) {
    parser_deallocate(parser, vtr->data);
  }
}

static void enlarge_vector_if_full(struct InternalParser* parser, Vector* vtr) {
  if (vtr->length >= vtr->capacity) {
    if (vtr->capacity) {
      size_t old_num_bytes = sizeof(void*) * vtr->capacity;
      vtr->capacity *= 2;
      size_t num_bytes = sizeof(void*) * vtr->capacity;
      void** temp = parser_allocate(parser, num_bytes);
      memcpy(temp, vtr->data, old_num_bytes);
      parser_deallocate(parser, vtr->data);
      vtr->data = temp;
    } else {
      // 0-capacity vector; no previous array to deallocate.
      vtr->capacity = 2;
      vtr->data = parser_allocate(parser, sizeof(void*) * vtr->capacity);
    }
  }
}

void vector_add(struct InternalParser* parser, void* element, Vector* vtr) {
  enlarge_vector_if_full(parser, vtr);
  assert(vtr->data);
  assert(vtr->length < vtr->capacity);
  vtr->data[vtr->length++] = element;
}

void* vector_pop(struct InternalParser* parser, Vector* vtr) {
    void *result;
    if (vtr->length == 0) {
        return NULL;
    }
    else{
        result = vtr->data[--vtr->length];
    }
    return result;
}

int vector_index_of(Vector* vtr, void* element) {
  for (int i = 0; i < vtr->length; ++i) {
    if (vtr->data[i] == element) {
      return i;
    }
  }
  return -1;
}

void vector_insert_at(struct InternalParser* parser, void* element, int index, Vector* vtr) {
  assert(index >= 0);
  assert(index <= vtr->length);
  enlarge_vector_if_full(parser, vtr);
  ++vtr->length;
  memmove(&vtr->data[index + 1], &vtr->data[index], sizeof(void*) * (vtr->length - index - 1));
  vtr->data[index] = element;
}

void vector_remove(struct InternalParser* parser, void* node, Vector* vtr) {
  int index = vector_index_of(vtr, node);
  if (index == -1) {
    return;
  }
  vector_remove_at(parser, index, vtr);
}

void* vector_remove_at(struct InternalParser* parser, int index, Vector* vtr) {
  assert(index >= 0);
  assert(index < vtr->length);
  void* result = vtr->data[index];
  memmove(&vtr->data[index], &vtr->data[index + 1], sizeof(void*) * (vtr->length - index - 1));
  --vtr->length;
  return result;
}
