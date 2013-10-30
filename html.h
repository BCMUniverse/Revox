#ifndef HTML_H_INCLUDED
#define HTML_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
    unsigned int line;
    unsigned int column;
    unsigned int offset;
} SrcPosition;

extern const SrcPosition kEmptySrcPosition;

typedef struct{
    const char* data;
    size_t length;
}StringPiece;

extern const StringPiece kEmptyString;
bool string_equals(const StringPiece* str1, const StringPiece* str2);
bool string_equals_ignore_case(const StringPiece* str1, const StringPiece* str2);

typedef struct{
    void** data;
    unsigned int length;
    unsigned int capacity;
}Vector;

extern const Vector kEmptyVector;
int vector_index_of(Vector* vtr1, void* element);

typedef enum {
    TAG_HTML,   //Elemento Principal - Root Element
    //Elementos do Cabeçalho do Arquivo
    TAG_HEAD,
    TAG_TITLE,
    TAG_BASE,
    TAG_LINK,
    TAG_META,
    TAG_STYLE,
    TAG_SCRIPT,
    TAG_NOSCRIPT,
    //Elementos do Corpo do Arquivo
    TAG_BODY,
    TAG_SECTION,
    TAG_NAV,
    TAG_ARTICLE,
    TAG_ASIDE,
    TAG_H1,
    TAG_H2,
    TAG_H3,
    TAG_H4,
    TAG_H5,
    TAG_H6,
    TAG_HGROUP,
    TAG_HEADER,
    TAG_FOOTER,
    TAG_ADDRESS,
    TAG_P,
    TAG_HR,
    TAG_PRE,
    TAG_BLOCKQUOTE,
    TAG_OL,
    TAG_UL,
    TAG_LI,
    TAG_DL,
    TAG_DT,
    TAG_DD,
    TAG_FIGURE,
    TAG_FIGCAPTION,
    TAG_DIV,
    TAG_A,
    TAG_EM,
    TAG_STRONG,
    TAG_SMALL,
    TAG_S,
    TAG_CITE,
    TAG_Q,
    TAG_DFN,
    TAG_ABBR,
    TAG_TIME,
    TAG_CODE,
    TAG_VAR,
    TAG_SAMP,
    TAG_KBD,
    TAG_SUB,
    TAG_SUP,
    TAG_I,
    TAG_B,
    TAG_MARK,
    TAG_RUBY,
    TAG_RT,
    TAG_RP,
    TAG_BDI,
    TAG_BDO,
    TAG_SPAN,
    TAG_BR,
    TAG_WBR,
    TAG_INS,
    TAG_DEL,
    TAG_IMAGE,
    TAG_IMG,
    TAG_IFRAME,
    TAG_EMBED,
    TAG_OBJECT,
    TAG_PARAM,
    TAG_VIDEO,
    TAG_AUDIO,
    TAG_SOURCE,
    TAG_TRACK,
    TAG_CANVAS,
    TAG_MAP,
    TAG_AREA,
    TAG_MATH,
    TAG_MI,
    TAG_MO,
    TAG_MN,
    TAG_MS,
    TAG_MTEXT,
    TAG_MGLYPH,
    TAG_MALIGNMARK,
    TAG_ANNOTATION_XML,
    TAG_SVG,
    TAG_FOREIGNOBJECT,
    TAG_DESC,
    TAG_TABLE,
    TAG_CAPTION,
    TAG_COLGROUP,
    TAG_COL,
    TAG_TBODY,
    TAG_THEAD,
    TAG_TFOOT,
    TAG_TR,
    TAG_TD,
    TAG_TH,
    TAG_FORM,
    TAG_FIELDSET,
    TAG_LEGEND,
    TAG_LABEL,
    TAG_INPUT,
    TAG_BUTTON,
    TAG_SELECT,
    TAG_DATALIST,
    TAG_OPTGROUP,
    TAG_OPTION,
    TAG_TEXTAREA,
    TAG_KEYGEN,
    TAG_OUTPUT,
    TAG_PROGRESS,
    TAG_METER,
    TAG_DETAILS,
    TAG_SUMMARY,
    TAG_COMMAND,
    TAG_MENU,
    TAG_APPLET,
    TAG_ACRONYM,
    TAG_BGSOUND,
    TAG_DIR,
    TAG_FRAME,
    TAG_FRAMESET,
    TAG_NOFRAMES,
    TAG_ISINDEX,
    TAG_LISTING,
    TAG_XMP,
    TAG_NEXTID,
    TAG_NOEMBED,
    TAG_PLAINTEXT,
    TAG_RB,
    TAG_STRIKE,
    TAG_BASEFONT,
    TAG_BIG,
    TAG_BLINK,
    TAG_CENTER,
    TAG_FONT,
    TAG_MARQUEE,
    TAG_MULTICOL,
    TAG_NOBR,
    TAG_SPACER,
    TAG_TT,
    TAG_U,
    //Usado para todos os elementos não reconhecido como Elementos HTML
    TAG_UNKNOWN,
    TAG_LAST,
} HTMLTag;

const char* normalized_tagname(HTMLTag tag);
void tag_from_original_text(StringPiece* text);
const char* normalize_svg_tagname(const StringPiece* tagname);
/*const */HTMLTag tag_enum(const char* tagname);

typedef enum {
    ATTR_NAMESPACE_NONE,
    ATTR_NAMESPACE_XLINK,
    ATTR_NAMESPACE_XML,
    ATTR_NAMESPACE_XMLNS,
}AttributeNamespaceEnum;

typedef struct {
    AttributeNamespaceEnum attr_namespace;
    const char* name;
    StringPiece original_name;
    const char* value;
    StringPiece original_value;
    SrcPosition name_start;
    SrcPosition name_end;
    SrcPosition value_start;
    SrcPosition value_end;
}Attribute;

//Attribute* get_attribute(const struct Vector* attributes, const char* name);

typedef enum {
    NODE_DOCUMENT,
    NODE_ELEMENT,
    NODE_TEXT,
    NODE_CDATA,
    NODE_COMMENT,
    NODE_WHITESPACE
}NodeType;

typedef struct InternalNode Node;

typedef enum {
    DOCTYPE_NO_QUIRKS,
    DOCTYPE_QUIRKS,
    DOCTYPE_LIMITED_QUIRKS
}QuirksModeEnum;

typedef enum {
    NAMESPACE_HTML,
    NAMESPACE_SVG,
    NAMESPACE_MATHML
}NameSpaceEnum;

typedef enum {
    INSERTION_NORMAL = 0,
    INSERTION_BY_PARSER = 1 << 0,
    INSERTION_IMPLICIT_END_TAG = 1 << 1,
    INSERTION_IMPLIED = 1 << 3,
    INSERTION_CONVERTED_FROM_END_TAG = 1 << 4,
    INSERTION_FROM_ISINDEX = 1 << 5,
    INSERTION_FROM_IMAGE = 1 << 6,
    INSERTION_RECONSTRUCTED_FORMATTING_ELEMENT = 1 << 7,
    INSERTION_ADOPTION_AGENCY_CLONED = 1 << 8,
    INSERTION_ADOPTION_AGENCY_MOVED = 1 << 9,
    INSERTION_FOSTER_PARENTED = 1 << 10
}ParseFlags;

typedef struct {
    Vector /* Node */ children;
    bool has_doctype;
    const char* name;
    const char* public_identifier;
    const char* system_identifier;
    QuirksModeEnum doc_type_quirks_mode;
}Document;

typedef struct {
    const char* text;
    StringPiece original_text;
    SrcPosition start_pos;
}Text;

typedef struct {
    Vector /* Node */ children;
    HTMLTag tag;
    NameSpaceEnum tag_namespace;
    StringPiece original_tag;
    StringPiece original_end_tag;
    SrcPosition start_pos;
    SrcPosition end_pos;
    Vector /* Attribute */ attributes;
}Element;

struct InternalNode{
    NodeType type;
    Node* parent;
    size_t index_within_parent;
    ParseFlags parse_flags;
    union{
        Document document;
        Element element;
        Text text;
    } v;
};

typedef void* (*AllocatorFunction)(void* userdata, size_t size);
typedef void (*DeallocatorFunction)(void* userdata, void* ptr);

typedef struct InternalOptions{
    AllocatorFunction allocator;
    DeallocatorFunction deallocator;
    void* userdata;
    int tab_stop;
    bool stop_on_first_error;
    int max_errors;
}Options;

extern const Options kDefaultOptions;

typedef struct InternalOutput{
    Node* document;
    Node* root;
    Vector /* Error */ errors;
}Output;

Output* parse(const char *buffer);
Output* parse_with_options(const Options* options, const char* buffer, size_t buffer_length);

//void* destroy_output(const Options* oprtions, Output* output);

#ifdef __cplusplus
}
#endif

#endif // HTML_H_INCLUDED
