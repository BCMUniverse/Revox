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
#include "string_buffer.h"
#include "string_piece.h"
#include "util.h"

struct InternalParser;

static const size_t kDefaultStringBufferSize = 10;

static void maybe_resize_string_buffer(struct InternalParser* parser, size_t additional_chars, StringBuffer* buffer) {
  size_t new_length = buffer->length + additional_chars;
  size_t new_capacity = buffer->capacity;
  while (new_capacity < new_length) {
    new_capacity *= 2;
  }
  if (new_capacity != buffer->capacity) {
    char* new_data = parser_allocate(parser, new_capacity);
    memcpy(new_data, buffer->data, buffer->length);
    parser_deallocate(parser, buffer->data);
    buffer->data = new_data;
    buffer->capacity = new_capacity;
  }
}

void string_buffer_init(struct InternalParser* parser, StringBuffer* output) {
  output->data = parser_allocate(parser, kDefaultStringBufferSize);
  output->length = 0;
  output->capacity = kDefaultStringBufferSize;
}

void string_buffer_reserve(struct InternalParser* parser, size_t min_capacity, StringBuffer* output) {
  maybe_resize_string_buffer(parser, min_capacity - output->length, output);
}

void string_buffer_append_codepoint(struct InternalParser* parser, int c, StringBuffer* output) {
  int num_bytes, prefix;
  if (c <= 0x7f) {
    num_bytes = 0;
    prefix = 0;
  } else if (c <= 0x7ff) {
    num_bytes = 1;
    prefix = 0xc0;
  } else if (c <= 0xffff) {
    num_bytes = 2;
    prefix = 0xe0;
  } else {
    num_bytes = 3;
    prefix = 0xf0;
  }
  maybe_resize_string_buffer(parser, num_bytes + 1, output);
  output->data[output->length++] = prefix | (c >> (num_bytes * 6));
  for (int i = num_bytes - 1; i >= 0; --i) {
    output->data[output->length++] = 0x80 | (0x3f & (c >> (i * 6)));
  }
}

void string_buffer_append_string(struct InternalParser* parser, StringPiece* str, StringBuffer* output) {
  maybe_resize_string_buffer(parser, str->length, output);
  memcpy(output->data + output->length, str->data, str->length);
  output->length += str->length;
}

char* string_buffer_to_string(struct InternalParser* parser, StringBuffer* input) {
  char* buffer = parser_allocate(parser, input->length + 1);
  memcpy(buffer, input->data, input->length);
  buffer[input->length] = '\0';
  return buffer;
}

void string_buffer_destroy(struct InternalParser* parser, StringBuffer* buffer) {
  parser_deallocate(parser, buffer->data);
}
