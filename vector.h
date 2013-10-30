#ifndef VECTOR_H_
#define VECTOR_H_

#include "html.h"

#ifdef __cplusplus
extern "C"{
#endif

struct InternalParser;

void vector_init(struct InternalParser* parser, size_t initial_capacity, Vector* vtr);
void vector_destroy(struct InternalParser* parser, Vector* vtr);
void vector_add(struct InternalParser* parser, void* element, Vector* vtr);
void* vector_pop(struct InternalParser* parser, Vector* vtr);
void vector_insert_at(struct InternalParser* parser, void* element, int index, Vector* vtr);
void vector_remove(struct InternalParser* parser, void* element, Vector* vtr);
void* vector_remove_at(struct InternalParser* parser, int index, Vector* vtr);

#ifdef __cplusplus
}
#endif

#endif // VECTOR_H_
