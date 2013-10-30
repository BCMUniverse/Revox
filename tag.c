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
