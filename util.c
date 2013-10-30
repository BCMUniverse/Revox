#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <stdio.h>
#include "html.h"
#include "parser.h"
#include "util.h"

const SrcPosition kEmptySrcPosition = {0, 0, 0};

void* parser_allocate(struct InternalParser* parser, size_t num_bytes){
    return parser->_options->allocator(parser->_options->userdata, num_bytes);
}
void parser_deallocate(struct InternalParser* parser, void* ptr){
    return parser->_options->deallocator(parser->_options->userdata, ptr);
}
char* copy_stringz(struct InternalParser* parser, const char* str){
    char* buffer = parser_allocate(parser, strlen(str)+1);
    strcpy(buffer, str);
    return buffer;
}
void debug(const char* format, ...){
    #ifdef DEBUG
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
    #endif // DEBUG
}
