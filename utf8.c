#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include "utf8.h"
#include "error.h"
#include "html.h"
#include "parser.h"
#include "util.h"
#include "vector.h"

const int kUtf8ReplacementChar = 0xFFFD;

static void add_error1(Utf8Iterator* iter, ErrorType type){
    Parser* parser = iter->_parser;

    Error* error = add_error(parser);
    if (!error){
        return;
    }
    error->type = type;
    error->position = iter->_pos;
    error->original_text = iter->_start;

    uint64_t code_point = 0;
    for (int i = 0; i < iter->_width; ++i){
        code_point = (code_point << 8) | (unsigned char) iter->_start[i];
    }
    error->v.codepoint = code_point;
}

static void read_char(Utf8Iterator* iter){
    unsigned char c;
    unsigned char mask = '\0';
    int is_bad_char = false;

    c = (unsigned char) *iter->_start;
    if (c < 0x80){
        iter->_width = 1;
        mask = 0xFF;
    }
    else if (c < 0xC0){
        iter->_width = 1;
        is_bad_char = true;
    }
    else if (c < 0xE0){
        iter->_width = 2;
        mask = 0x1F;
        if (c < 0xC2){
            is_bad_char = true;
        }
    }
    else if (c < 0xF0){
        iter->_width = 3;
        mask = 0xF;
    }
    else if (c < 0xF5){
        iter->_width = 4;
        mask = 0x7;
    }
    else if (c < 0xF8){
        iter->_width = 5;
        is_bad_char = true;
    }
    else if (c < 0xFC){
        iter->_width = 6;
        is_bad_char = true;
    }
    else if (c < 0xFE){
        iter->_width = 7;
        is_bad_char = true;
    }
    else{
        iter->_width = 1;
        is_bad_char = true;
    }
    uint64_t code_point = c & mask;
    if (iter->_start + iter->_width > iter->_end){
        iter->_width = iter->_end - iter->_start;
        add_error1(iter, ERR_UTF8_TRUNCATED);
        is_bad_char = true;
    }

    assert(iter->_width < 8);
    for (int i = 1; i < iter->_width; ++i){
        c = (unsigned char) iter->_start[i];
        if (c < 0x80 || c > 0xBF){
            iter->_width = i;
            is_bad_char = true;
            break;
        }
        code_point = (code_point << 6) | (c & ~0x80);
    }
    if (code_point > 0x10FFFF) is_bad_char = true;

    if (is_bad_char || utf8_is_invalid_code_point(code_point)){
        add_error1(iter, ERR_UTF8_INVALID);
        code_point = kUtf8ReplacementChar;
    }

    if (code_point == '\r'){
        const char* next = iter->_start + iter->_width;
        if (next < iter->_end && *next == '\n'){
            ++iter->_start;
            ++iter->_pos.offset;
        }
        code_point = '\n';
    }

    iter->_current = code_point;
}

static void update_position(Utf8Iterator* iter){
    iter->_pos.offset += iter->_width;
    if (iter->_current == '\n'){
        ++iter->_pos.line;
        iter->_pos.column = 1;
    }
    else if(iter->_current == '\t'){
        int tab_stop = iter->_parser->_options->tab_stop;
        iter->_pos.column = ((iter->_pos.column / tab_stop) + 1) * tab_stop;
    }
    else{
        ++iter->_pos.column;
    }
}

bool utf8_is_invalid_code_point(int c){
    return (c >= 0x1 && c <= 0x8) || c == 0xB || (c >= 0xE && c <= 0x1F) ||
           (c >= 0x7F && c <= 0x9F) || (c >= 0xFDD0 && c <= 0xFDEF) ||
           ((c & 0xFFFF) == 0xFFFE) || ((c & 0xFFFF) == 0xFFFF);
}

void utf8iterator_init(Parser* parser, const char* source, size_t source_length, Utf8Iterator* iter){
    iter->_start = source;
    iter->_end = source + source_length;
    iter->_width = 0;
    iter->_pos.line = 1;
    iter->_pos.column = 1;
    iter->_pos.offset = 0;
    iter->_parser = parser;
    if (source_length){
        read_char(iter);
    }
    else{
        iter->_current = -1;
    }
}

void utf8iterator_next(Utf8Iterator* iter){
    iter->_start += iter->_width;
    update_position(iter);
    if (iter->_start < iter->_end){
        read_char(iter);
    }
    else{
        iter->_current = -1;
    }
}

int utf8iterator_current(const Utf8Iterator* iter){
    return iter->_current;
}

void utf8iterator_get_position(const Utf8Iterator* iter, SrcPosition* output){
    *output = iter->_pos;
}

const char* utf8iterator_get_char_pointer(const Utf8Iterator* iter){
    return iter->_start;
}

bool utf8iterator_maybe_consume_match(
    Utf8Iterator* iter, const char* prefix, size_t length,
    bool case_sensitive){
    bool matched = (iter->_start + length <= iter->_end) && (case_sensitive ?
                   !strncmp(iter->_start, prefix, length) :
                   !strncasecmp(iter->_start, prefix, length));
    if (matched){
        for (int i = 0; i < length; ++i){
            utf8iterator_next(iter);
        }
        return true;
    }
    else{
        return false;
    }
}

void utf8iterator_mark(Utf8Iterator* iter){
    iter->_mark = iter->_start;
    iter->_mark_pos = iter->_pos;
}
void utf8iterator_reset(Utf8Iterator* iter){
    iter->_start = iter->_mark;
    iter->_pos = iter->_mark_pos;
    read_char(iter);
}
void utf8iterator_fill_error_at_mark(Utf8Iterator* iter, Error* error){
    error->position = iter->_mark_pos;
    error->original_text = iter->_mark;
}
