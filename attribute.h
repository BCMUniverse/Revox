#ifndef ATTRIBUTE_H_
#define ATTRIBUTE_H_

#include "html.h"

#ifdef __cplusplus
extern "C"{
#endif

struct InternalParser;

Attribute* get_attribute(const Vector* attributes, const char* name);
void destroy_attribute(struct InternalParser* parser, Attribute* attribute);

#ifdef __cplusplus
}
#endif

#endif // ATTRIBUTE_H_
