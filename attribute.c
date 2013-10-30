#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "attribute.h"
#include "html.h"
#include "util.h"

struct InternalParser;

Attribute* get_attribute(const Vector* attributes, const char* name) {
  for (int i = 0; i < attributes->length; ++i) {
    Attribute* attr = (Attribute*)attributes->data[i];
    if (!strcasecmp(attr->name, name)) {
      return attr;
    }
  }
  return NULL;
}

void destroy_attribute(struct InternalParser* parser, Attribute* attribute) {
  parser_deallocate(parser, (void*) attribute->name);
  parser_deallocate(parser, (void*) attribute->value);
  parser_deallocate(parser, (void*) attribute);
}
