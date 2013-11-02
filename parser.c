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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "attribute.h"
#include "error.h"
#include "html.h"
#include "insertion_mode.h"
#include "parser.h"
#include "tokenizer.h"
#include "tokenizer_states.h"
#include "utf8.h"
#include "util.h"
#include "vector.h"


#define AVOID_UNUSED_VARIABLE_WARNING(i) (void)(i)

#define STRING(literal) { literal, sizeof(literal) - 1 }
#define TERMINATOR { "", 0 }

static void* malloc_wrapper(void* unused, size_t size) {
  return malloc(size);
}

static void free_wrapper(void* unused, void* ptr) {
  return free(ptr);
}

const Options kDefaultOptions = {
  &malloc_wrapper,
  &free_wrapper,
  NULL,
  8,
  false,
  -1,
};

static const StringPiece kDoctypeHtml = STRING("html");
static const StringPiece kPublicIdHtml4_0 = STRING("-//W3C//DTD HTML 4.0//EN");
static const StringPiece kPublicIdHtml4_01 = STRING("-//W3C//DTD HTML 4.01//EN");
static const StringPiece kPublicIdXhtml1_0 = STRING("-//W3C//DTD XHTML 1.0 Strict//EN");
static const StringPiece kPublicIdXhtml1_1 = STRING("-//W3C//DTD XHTML 1.1//EN");
static const StringPiece kSystemIdRecHtml4_0 = STRING("http://www.w3.org/TR/REC-html40/strict.dtd");
static const StringPiece kSystemIdHtml4 = STRING("http://www.w3.org/TR/html4/strict.dtd");
static const StringPiece kSystemIdXhtmlStrict1_1 = STRING("http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd");
static const StringPiece kSystemIdXhtml1_1 = STRING("http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd");
static const StringPiece kSystemIdLegacyCompat = STRING("about:legacy-compat");

static const StringPiece kQuirksModePublicIdPrefixes[] = {
  STRING("+//Silmaril//dtd html Pro v0r11 19970101//"),
  STRING("-//AdvaSoft Ltd//DTD HTML 3.0 asWedit + extensions//"),
  STRING("-//AS//DTD HTML 3.0 asWedit + extensions//"),
  STRING("-//IETF//DTD HTML 2.0 Level 1//"),
  STRING("-//IETF//DTD HTML 2.0 Level 2//"),
  STRING("-//IETF//DTD HTML 2.0 Strict Level 1//"),
  STRING("-//IETF//DTD HTML 2.0 Strict Level 2//"),
  STRING("-//IETF//DTD HTML 2.0 Strict//"),
  STRING("-//IETF//DTD HTML 2.0//"),
  STRING("-//IETF//DTD HTML 2.1E//"),
  STRING("-//IETF//DTD HTML 3.0//"),
  STRING("-//IETF//DTD HTML 3.2 Final//"),
  STRING("-//IETF//DTD HTML 3.2//"),
  STRING("-//IETF//DTD HTML 3//"),
  STRING("-//IETF//DTD HTML Level 0//"),
  STRING("-//IETF//DTD HTML Level 1//"),
  STRING("-//IETF//DTD HTML Level 2//"),
  STRING("-//IETF//DTD HTML Level 3//"),
  STRING("-//IETF//DTD HTML Strict Level 0//"),
  STRING("-//IETF//DTD HTML Strict Level 1//"),
  STRING("-//IETF//DTD HTML Strict Level 2//"),
  STRING("-//IETF//DTD HTML Strict Level 3//"),
  STRING("-//IETF//DTD HTML Strict//"),
  STRING("-//IETF//DTD HTML//"),
  STRING("-//Metrius//DTD Metrius Presentational//"),
  STRING("-//Microsoft//DTD Internet Explorer 2.0 HTML Strict//"),
  STRING("-//Microsoft//DTD Internet Explorer 2.0 HTML//"),
  STRING("-//Microsoft//DTD Internet Explorer 2.0 Tables//"),
  STRING("-//Microsoft//DTD Internet Explorer 3.0 HTML Strict//"),
  STRING("-//Microsoft//DTD Internet Explorer 3.0 HTML//"),
  STRING("-//Microsoft//DTD Internet Explorer 3.0 Tables//"),
  STRING("-//Netscape Comm. Corp.//DTD HTML//"),
  STRING("-//Netscape Comm. Corp.//DTD Strict HTML//"),
  STRING("-//O'Reilly and Associates//DTD HTML 2.0//"),
  STRING("-//O'Reilly and Associates//DTD HTML Extended 1.0//"),
  STRING("-//O'Reilly and Associates//DTD HTML Extended Relaxed 1.0//"),
  STRING("-//SoftQuad Software//DTD HoTMetaL PRO 6.0::19990601::)"
      "extensions to HTML 4.0//"),
  STRING("-//SoftQuad//DTD HoTMetaL PRO 4.0::19971010::"
      "extensions to HTML 4.0//"),
  STRING("-//Spyglass//DTD HTML 2.0 Extended//"),
  STRING("-//SQ//DTD HTML 2.0 HoTMetaL + extensions//"),
  STRING("-//Sun Microsystems Corp.//DTD HotJava HTML//"),
  STRING("-//Sun Microsystems Corp.//DTD HotJava Strict HTML//"),
  STRING("-//W3C//DTD HTML 3 1995-03-24//"),
  STRING("-//W3C//DTD HTML 3.2 Draft//"),
  STRING("-//W3C//DTD HTML 3.2 Final//"),
  STRING("-//W3C//DTD HTML 3.2//"),
  STRING("-//W3C//DTD HTML 3.2S Draft//"),
  STRING("-//W3C//DTD HTML 4.0 Frameset//"),
  STRING("-//W3C//DTD HTML 4.0 Transitional//"),
  STRING("-//W3C//DTD HTML Experimental 19960712//"),
  STRING("-//W3C//DTD HTML Experimental 970421//"),
  STRING("-//W3C//DTD W3 HTML//"),
  STRING("-//W3O//DTD W3 HTML 3.0//"),
  STRING("-//WebTechs//DTD Mozilla HTML 2.0//"),
  STRING("-//WebTechs//DTD Mozilla HTML//"),
  TERMINATOR
};

static const StringPiece kQuirksModePublicIdExactMatches[] = {
  STRING("-//W3O//DTD W3 HTML Strict 3.0//EN//"),
  STRING("-/W3C/DTD HTML 4.0 Transitional/EN"),
  STRING("HTML"),
  TERMINATOR
};

static const StringPiece kQuirksModeSystemIdExactMatches[] = {
  STRING("http://www.ibm.com/data/dtd/v11/ibmxhtml1-transitional.dtd"),
  TERMINATOR
};

static const StringPiece kLimitedQuirksPublicIdPrefixes[] = {
  STRING("-//W3C//DTD XHTML 1.0 Frameset//"),
  STRING("-//W3C//DTD XHTML 1.0 Transitional//"),
  TERMINATOR
};

static const StringPiece kLimitedQuirksRequiresSystemIdPublicIdPrefixes[] = {
  STRING("-//W3C//DTD HTML 4.01 Frameset//"),
  STRING("-//W3C//DTD HTML 4.01 Transitional//"),
  TERMINATOR
};

// Indexed by GumboNamespaceEnum; keep in sync with that.
static const char* kLegalXmlns[] = {
  "http://www.w3.org/1999/xhtml",
  "http://www.w3.org/2000/svg",
  "http://www.w3.org/1998/Math/MathML"
};

typedef struct _ReplacementEntry {
  const StringPiece from;
  const StringPiece to;
} ReplacementEntry;

#define REPLACEMENT_ENTRY(from, to) \
    { STRING(from), STRING(to) }

// Static data for SVG attribute replacements.
// http://www.whatwg.org/specs/web-apps/current-work/multipage/tree-construction.html#adjust-svg-attributes
static const ReplacementEntry kSvgAttributeReplacements[] = {
  REPLACEMENT_ENTRY("attributename", "attributeName"),
  REPLACEMENT_ENTRY("attributetype", "attributeType"),
  REPLACEMENT_ENTRY("basefrequency", "baseFrequency"),
  REPLACEMENT_ENTRY("baseprofile", "baseProfile"),
  REPLACEMENT_ENTRY("calcmode", "calcMode"),
  REPLACEMENT_ENTRY("clippathunits", "clipPathUnits"),
  REPLACEMENT_ENTRY("contentscripttype", "contentScriptType"),
  REPLACEMENT_ENTRY("contentstyletype", "contentStyleType"),
  REPLACEMENT_ENTRY("diffuseconstant", "diffuseConstant"),
  REPLACEMENT_ENTRY("edgemode", "edgeMode"),
  REPLACEMENT_ENTRY("externalresourcesrequired", "externalResourcesRequired"),
  REPLACEMENT_ENTRY("filterres", "filterRes"),
  REPLACEMENT_ENTRY("filterunits", "filterUnits"),
  REPLACEMENT_ENTRY("glyphref", "glyphRef"),
  REPLACEMENT_ENTRY("gradienttransform", "gradientTransform"),
  REPLACEMENT_ENTRY("gradientunits", "gradientUnits"),
  REPLACEMENT_ENTRY("kernelmatrix", "kernelMatrix"),
  REPLACEMENT_ENTRY("kernelunitlength", "kernelUnitLength"),
  REPLACEMENT_ENTRY("keypoints", "keyPoints"),
  REPLACEMENT_ENTRY("keysplines", "keySplines"),
  REPLACEMENT_ENTRY("keytimes", "keyTimes"),
  REPLACEMENT_ENTRY("lengthadjust", "lengthAdjust"),
  REPLACEMENT_ENTRY("limitingconeangle", "limitingConeAngle"),
  REPLACEMENT_ENTRY("markerheight", "markerHeight"),
  REPLACEMENT_ENTRY("markerunits", "markerUnits"),
  REPLACEMENT_ENTRY("markerwidth", "markerWidth"),
  REPLACEMENT_ENTRY("maskcontentunits", "maskContentUnits"),
  REPLACEMENT_ENTRY("maskunits", "maskUnits"),
  REPLACEMENT_ENTRY("numoctaves", "numOctaves"),
  REPLACEMENT_ENTRY("pathlength", "pathLength"),
  REPLACEMENT_ENTRY("patterncontentunits", "patternContentUnits"),
  REPLACEMENT_ENTRY("patterntransform", "patternTransform"),
  REPLACEMENT_ENTRY("patternunits", "patternUnits"),
  REPLACEMENT_ENTRY("pointsatx", "pointsAtX"),
  REPLACEMENT_ENTRY("pointsaty", "pointsAtY"),
  REPLACEMENT_ENTRY("pointsatz", "pointsAtZ"),
  REPLACEMENT_ENTRY("preservealpha", "preserveAlpha"),
  REPLACEMENT_ENTRY("preserveaspectratio", "preserveAspectRatio"),
  REPLACEMENT_ENTRY("primitiveunits", "primitiveUnits"),
  REPLACEMENT_ENTRY("refx", "refX"),
  REPLACEMENT_ENTRY("refy", "refY"),
  REPLACEMENT_ENTRY("repeatcount", "repeatCount"),
  REPLACEMENT_ENTRY("repeatdur", "repeatDur"),
  REPLACEMENT_ENTRY("requiredextensions", "requiredExtensions"),
  REPLACEMENT_ENTRY("requiredfeatures", "requiredFeatures"),
  REPLACEMENT_ENTRY("specularconstant", "specularConstant"),
  REPLACEMENT_ENTRY("specularexponent", "specularExponent"),
  REPLACEMENT_ENTRY("spreadmethod", "spreadMethod"),
  REPLACEMENT_ENTRY("startoffset", "startOffset"),
  REPLACEMENT_ENTRY("stddeviation", "stdDeviation"),
  REPLACEMENT_ENTRY("stitchtiles", "stitchTiles"),
  REPLACEMENT_ENTRY("surfacescale", "surfaceScale"),
  REPLACEMENT_ENTRY("systemlanguage", "systemLanguage"),
  REPLACEMENT_ENTRY("tablevalues", "tableValues"),
  REPLACEMENT_ENTRY("targetx", "targetX"),
  REPLACEMENT_ENTRY("targety", "targetY"),
  REPLACEMENT_ENTRY("textlength", "textLength"),
  REPLACEMENT_ENTRY("viewbox", "viewBox"),
  REPLACEMENT_ENTRY("viewtarget", "viewTarget"),
  REPLACEMENT_ENTRY("xchannelselector", "xChannelSelector"),
  REPLACEMENT_ENTRY("ychannelselector", "yChannelSelector"),
  REPLACEMENT_ENTRY("zoomandpan", "zoomAndPan"),
};

static const ReplacementEntry kSvgTagReplacements[] = {
  REPLACEMENT_ENTRY("altglyph", "altGlyph"),
  REPLACEMENT_ENTRY("altglyphdef", "altGlyphDef"),
  REPLACEMENT_ENTRY("altglyphitem", "altGlyphItem"),
  REPLACEMENT_ENTRY("animatecolor", "animateColor"),
  REPLACEMENT_ENTRY("animatemotion", "animateMotion"),
  REPLACEMENT_ENTRY("animatetransform", "animateTransform"),
  REPLACEMENT_ENTRY("clippath", "clipPath"),
  REPLACEMENT_ENTRY("feblend", "feBlend"),
  REPLACEMENT_ENTRY("fecolormatrix", "feColorMatrix"),
  REPLACEMENT_ENTRY("fecomponenttransfer", "feComponentTransfer"),
  REPLACEMENT_ENTRY("fecomposite", "feComposite"),
  REPLACEMENT_ENTRY("feconvolvematrix", "feConvolveMatrix"),
  REPLACEMENT_ENTRY("fediffuselighting", "feDiffuseLighting"),
  REPLACEMENT_ENTRY("fedisplacementmap", "feDisplacementMap"),
  REPLACEMENT_ENTRY("fedistantlight", "feDistantLight"),
  REPLACEMENT_ENTRY("feflood", "feFlood"),
  REPLACEMENT_ENTRY("fefunca", "feFuncA"),
  REPLACEMENT_ENTRY("fefuncb", "feFuncB"),
  REPLACEMENT_ENTRY("fefuncg", "feFuncG"),
  REPLACEMENT_ENTRY("fefuncr", "feFuncR"),
  REPLACEMENT_ENTRY("fegaussianblur", "feGaussianBlur"),
  REPLACEMENT_ENTRY("feimage", "feImage"),
  REPLACEMENT_ENTRY("femerge", "feMerge"),
  REPLACEMENT_ENTRY("femergenode", "feMergeNode"),
  REPLACEMENT_ENTRY("femorphology", "feMorphology"),
  REPLACEMENT_ENTRY("feoffset", "feOffset"),
  REPLACEMENT_ENTRY("fepointlight", "fePointLight"),
  REPLACEMENT_ENTRY("fespecularlighting", "feSpecularLighting"),
  REPLACEMENT_ENTRY("fespotlight", "feSpotLight"),
  REPLACEMENT_ENTRY("fetile", "feTile"),
  REPLACEMENT_ENTRY("feturbulence", "feTurbulence"),
  REPLACEMENT_ENTRY("foreignobject", "foreignObject"),
  REPLACEMENT_ENTRY("glyphref", "glyphRef"),
  REPLACEMENT_ENTRY("lineargradient", "linearGradient"),
  REPLACEMENT_ENTRY("radialgradient", "radialGradient"),
  REPLACEMENT_ENTRY("textpath", "textPath"),
};

typedef struct _NamespacedAttributeReplacement {
  const char* from;
  const char* local_name;
  const AttributeNamespaceEnum attr_namespace;
} NamespacedAttributeReplacement;

static const NamespacedAttributeReplacement kForeignAttributeReplacements[] = {
  { "xlink:actuate", "actuate", ATTR_NAMESPACE_XLINK },
  { "xlink:actuate", "actuate", ATTR_NAMESPACE_XLINK },
  { "xlink:href", "href", ATTR_NAMESPACE_XLINK },
  { "xlink:role", "role", ATTR_NAMESPACE_XLINK },
  { "xlink:show", "show", ATTR_NAMESPACE_XLINK },
  { "xlink:title", "title", ATTR_NAMESPACE_XLINK },
  { "xlink:type", "type", ATTR_NAMESPACE_XLINK },
  { "xml:base", "base", ATTR_NAMESPACE_XML },
  { "xml:lang", "lang", ATTR_NAMESPACE_XML },
  { "xml:space", "space", ATTR_NAMESPACE_XML },
  { "xmlns", "xmlns", ATTR_NAMESPACE_XMLNS },
  { "xmlns:xlink", "xlink", ATTR_NAMESPACE_XMLNS },
};

static const Node kActiveFormattingScopeMarker = {};

static const bool kStartTag = true;
static const bool kEndTag = false;

typedef struct _TextNodeBufferState {
  StringBuffer _buffer;
  const char* _start_original_text;
  SrcPosition _start_position;
  NodeType _type;
} TextNodeBufferState;

typedef struct InternalParserState {
  InsertionMode _insertion_mode;
  InsertionMode _original_insertion_mode;
  Vector /*Node*/ _open_elements;
  Vector /*Node*/ _active_formatting_elements;
  Node* _head_element;
  Node* _form_element;
  bool _reprocess_current_token;
  bool _self_closing_flag_acknowledged;
  bool _frameset_ok;
  bool _ignore_next_linefeed;
  bool _foster_parent_insertions;
  TextNodeBufferState _text_node;
  Token* _current_token;
  bool _closed_body_tag;
  bool _closed_html_tag;
} ParserState;

static bool token_has_attribute(const Token* token, const char* name) {
  assert(token->type == TOKEN_START_TAG);
  return get_attribute(&token->v.start_tag.attributes, name) != NULL;
}

static bool attribute_matches(const Vector* attributes, const char* name, const char* value) {
  const Attribute* attr = get_attribute(attributes, name);
  return attr ? strcasecmp(value, attr->value) == 0 : false;
}

static bool attribute_matches_case_sensitive(const Vector* attributes, const char* name, const char* value) {
  const Attribute* attr = get_attribute(attributes, name);
  return attr ?  strcmp(value, attr->value) == 0 : false;
}

static bool all_attributes_match(const Vector* attr1, const Vector* attr2) {
  int num_unmatched_attr2_elements = attr2->length;
  for (int i = 0; i < attr1->length; ++i) {
    Attribute* attr = (Attribute*)attr1->data[i];
    if (attribute_matches_case_sensitive(attr2, attr->name, attr->value)) {
      --num_unmatched_attr2_elements;
    } else {
      return false;
    }
  }
  return num_unmatched_attr2_elements == 0;
}

static void set_frameset_not_ok(Parser* parser) {
  debug("Setting frameset_ok to false.\n");
  parser->_parser_state->_frameset_ok = false;
}

static Node* create_node(Parser* parser, NodeType type) {
  Node* node = (Node*)parser_allocate(parser, sizeof(Node));
  node->parent = NULL;
  node->index_within_parent = -1;
  node->type = type;
  node->parse_flags = INSERTION_NORMAL;
  return node;
}

static Node* new_document_node(Parser* parser) {
  Node* document_node = create_node(parser, NODE_DOCUMENT);
  document_node->parse_flags = INSERTION_BY_PARSER;
  vector_init(parser, 1, &document_node->v.document.children);
  Document* document = &document_node->v.document;
  document->has_doctype = false;
  document->name = NULL;
  document->public_identifier = NULL;
  document->system_identifier = NULL;
  return document_node;
}

static void output_init(Parser* parser) {
  Output* output = (Output*)parser_allocate(parser, sizeof(Output));
  output->root = NULL;
  output->document = new_document_node(parser);
  parser->_output = output;
  init_errors(parser);
}

static void parser_state_init(Parser* parser) {
  ParserState* parser_state = (ParserState*)parser_allocate(parser, sizeof(ParserState));
  parser_state->_insertion_mode = INSERTION_MODE_INITIAL;
  parser_state->_reprocess_current_token = false;
  parser_state->_frameset_ok = true;
  parser_state->_ignore_next_linefeed = false;
  parser_state->_foster_parent_insertions = false;
  parser_state->_text_node._type = NODE_WHITESPACE;
  string_buffer_init(parser, &parser_state->_text_node._buffer);
  vector_init(parser, 10, &parser_state->_open_elements);
  vector_init(parser, 5, &parser_state->_active_formatting_elements);
  parser_state->_head_element = NULL;
  parser_state->_form_element = NULL;
  parser_state->_current_token = NULL;
  parser_state->_closed_body_tag = false;
  parser_state->_closed_html_tag = false;
  parser->_parser_state = parser_state;
}

static void parser_state_destroy(Parser* parser) {
  ParserState* state = parser->_parser_state;
  vector_destroy(parser, &state->_active_formatting_elements);
  vector_destroy(parser, &state->_open_elements);
  string_buffer_destroy(parser, &state->_text_node._buffer);
  parser_deallocate(parser, state);
}

static Node* get_document_node(Parser* parser) {
  return parser->_output->document;
}

static Node* get_current_node(Parser* parser) {
  Vector* open_elements = &parser->_parser_state->_open_elements;
  if (open_elements->length == 0) {
    assert(!parser->_output->root);
    return NULL;
  }
  assert(open_elements->length > 0);
  assert(open_elements->data != NULL);
  return (Node*)open_elements->data[open_elements->length - 1];
}

static bool is_in_static_list(const char* needle, const StringPiece* haystack, bool exact_match) {
  for (int i = 0; haystack[i].length > 0; ++i) {
    if ((exact_match && !strcmp(needle, haystack[i].data)) ||
        (!exact_match && !strcasecmp(needle, haystack[i].data))) {
      return true;
    }
  }
  return false;
}

static void set_insertion_mode(Parser* parser, InsertionMode mode) {
  parser->_parser_state->_insertion_mode = mode;
}

static InsertionMode get_appropriate_insertion_mode(const Node *node, bool is_last) {
  assert(node->type == NODE_ELEMENT);
  switch (node->v.element.tag) {
    case TAG_SELECT:
      return INSERTION_MODE_IN_SELECT;
    case TAG_TD:
    case TAG_TH:
      return is_last ?
          INSERTION_MODE_IN_BODY : INSERTION_MODE_IN_CELL;
    case TAG_TR:
      return INSERTION_MODE_IN_ROW;
    case TAG_TBODY:
    case TAG_THEAD:
    case TAG_TFOOT:
      return INSERTION_MODE_IN_TABLE_BODY;
    case TAG_CAPTION:
      return INSERTION_MODE_IN_CAPTION;
    case TAG_COLGROUP:
      return INSERTION_MODE_IN_COLUMN_GROUP;
    case TAG_TABLE:
      return INSERTION_MODE_IN_TABLE;
    case TAG_HEAD:
    case TAG_BODY:
      return INSERTION_MODE_IN_BODY;
    case TAG_FRAMESET:
      return INSERTION_MODE_IN_FRAMESET;
    case TAG_HTML:
      return INSERTION_MODE_BEFORE_HEAD;
    default:
      return is_last ? INSERTION_MODE_IN_BODY : INSERTION_MODE_INITIAL;
  }
}

static void reset_insertion_mode_appropriately(Parser* parser) {
  const Vector* open_elements = &parser->_parser_state->_open_elements;
  for (int i = open_elements->length; --i >= 0; ) {
    InsertionMode mode = get_appropriate_insertion_mode((const Node*)open_elements->data[i], i == 0);
    if (mode != INSERTION_MODE_INITIAL) {
      set_insertion_mode(parser, mode);
      return;
    }
  }
  assert(0);
}

static Error* add_parse_error(Parser* parser, const Token* token) {
  debug("Adding parse error.\n");
  Error* error = add_error(parser);
  if (!error) {
    return NULL;
  }
  error->type = ERR_PARSER;
  error->position = token->position;
  error->original_text = token->original_text.data;
  ParserError* extra_data = &error->v.parser;
  extra_data->input_type = token->type;
  extra_data->input_tag = TAG_UNKNOWN;
  if (token->type == TOKEN_START_TAG) {
    extra_data->input_tag = token->v.start_tag.tag;
  } else if (token->type == TOKEN_END_TAG) {
    extra_data->input_tag = token->v.end_tag;
  }
  ParserState* state = parser->_parser_state;
  extra_data->parser_state = state->_insertion_mode;
  vector_init(parser, state->_open_elements.length, &extra_data->tag_stack);
  for (int i = 0; i < state->_open_elements.length; ++i) {
    const Node* node = (const Node*)state->_open_elements.data[i];
    assert(node->type == NODE_ELEMENT);
    vector_add(parser, (void*) node->v.element.tag, &extra_data->tag_stack);
  }
  return error;
}

static bool tag_in(const Token* token, bool is_start, ...) {
  HTMLTag token_tag;
  if (is_start && token->type == TOKEN_START_TAG) {
    token_tag = token->v.start_tag.tag;
  } else if (!is_start && token->type == TOKEN_END_TAG) {
    token_tag = token->v.end_tag;
  } else {
    return false;
  }

  va_list tags;
  va_start(tags, is_start);
  bool result = false;
  for (HTMLTag tag = va_arg(tags, HTMLTag); tag != TAG_LAST; tag = va_arg(tags, HTMLTag)) {
    if (tag == token_tag) {
      result = true;
      break;
    }
  }
  va_end(tags);
  return result;
}

static bool tag_is(const Token* token, bool is_start, HTMLTag tag) {
  if (is_start && token->type == TOKEN_START_TAG) {
    return token->v.start_tag.tag == tag;
  } else if (!is_start && token->type == TOKEN_END_TAG) {
    return token->v.end_tag == tag;
  } else {
    return false;
  }
}

static bool node_tag_in(const Node* node, ...) {
  assert(node != NULL);
  if (node->type != NODE_ELEMENT) {
    return false;
  }
  HTMLTag node_tag = node->v.element.tag;

  va_list tags;
  va_start(tags, node);
  bool result = false;
  for (HTMLTag tag = va_arg(tags, HTMLTag); tag != TAG_LAST; tag = va_arg(tags, HTMLTag)) {
    assert(tag <= TAG_LAST);
    if (tag == node_tag) {
      result = true;
      break;
    }
  }
  va_end(tags);
  return result;
}

static bool node_tag_is(const Node* node, HTMLTag tag) {
  return node->type == NODE_ELEMENT && node->v.element.tag == tag;
}

static bool is_mathml_integration_point(const Node* node) {
  return node_tag_in(node,TAG_MI, TAG_MO, TAG_MN, TAG_MS, TAG_MTEXT, TAG_LAST) && node->v.element.tag_namespace == NAMESPACE_MATHML;
}

static bool is_html_integration_point(const Node* node) {
  return (node_tag_in(node, TAG_FOREIGNOBJECT, TAG_DESC, TAG_TITLE, TAG_LAST) && node->v.element.tag_namespace == NAMESPACE_SVG) || (node_tag_is(node, TAG_ANNOTATION_XML) && (attribute_matches(&node->v.element.attributes,"encoding", "text/html") ||
        attribute_matches(&node->v.element.attributes, "encoding", "application/xhtml+xml")));
}

static void append_node(Parser* parser, Node* parent, Node* node) {
  assert(node->parent == NULL);
  assert(node->index_within_parent = -1);
  Vector* children;
  if (parent->type == NODE_ELEMENT) {
    children = &parent->v.element.children;
  } else {
    assert(parent->type == NODE_DOCUMENT);
    children = &parent->v.document.children;
  }
  node->parent = parent;
  node->index_within_parent = children->length;
  vector_add(parser, (void*) node, children);
  assert(node->index_within_parent < children->length);
}

static void insert_node(Parser* parser, Node* parent, int index, Node* node) {
  assert(node->parent == NULL);
  assert(node->index_within_parent = -1);
  assert(parent->type == NODE_ELEMENT);
  Vector* children = &parent->v.element.children;
  assert(index >= 0);
  assert(index < children->length);
  node->parent = parent;
  node->index_within_parent = index;
  vector_insert_at(parser, (void*) node, index, children);
  assert(node->index_within_parent < children->length);
  for (int i = index + 1; i < children->length; ++i) {
    Node* sibling = (Node*)children->data[i];
    sibling->index_within_parent = i;
    assert(sibling->index_within_parent < children->length);
  }
}

static void foster_parent_element(Parser* parser, Node* node) {
  Vector* open_elements = &parser->_parser_state->_open_elements;
  assert(open_elements->length > 2);

  node->parse_flags = INSERTION_FOSTER_PARENTED;
  Node* foster_parent_element = (Node*)open_elements->data[0];
  assert(foster_parent_element->type == NODE_ELEMENT);
  assert(node_tag_is(foster_parent_element, TAG_HTML));
  for (int i = open_elements->length; --i > 1; ) {
    Node* table_element = (Node*)open_elements->data[i];
    if (node_tag_is(table_element, TAG_TABLE)) {
      foster_parent_element = table_element->parent;
      if (!foster_parent_element ||
          foster_parent_element->type != NODE_ELEMENT) {
        // Table has no parent; spec says it's possible if a script manipulated
        // the DOM, although I don't think we have to worry about this case.
        debug("Table has no parent.\n");
        foster_parent_element = (Node*)open_elements->data[i - 1];
        break;
      }
      assert(foster_parent_element->type == NODE_ELEMENT);
      debug("Found enclosing table (%x) at %d; parent=%s, index=%d.\n", table_element, i, normalized_tagname(foster_parent_element->v.element.tag), table_element->index_within_parent);
      assert(foster_parent_element->v.element.children.data[table_element->index_within_parent] == table_element);
      insert_node(parser, foster_parent_element, table_element->index_within_parent, node);
      return;
    }
  }
  if (node->type == NODE_ELEMENT) {
    vector_add(parser, (void*) node, open_elements);
  }
  append_node(parser, foster_parent_element, node);
}

static void maybe_flush_text_node_buffer(Parser* parser) {
  ParserState* state = parser->_parser_state;
  TextNodeBufferState* buffer_state = &state->_text_node;
  if (buffer_state->_buffer.length == 0) {
    return;
  }

  assert(buffer_state->_type == NODE_WHITESPACE || buffer_state->_type == NODE_TEXT);
  Node* text_node = create_node(parser, buffer_state->_type);
  Text* text_node_data = &text_node->v.text;
  text_node_data->text = string_buffer_to_string(parser, &buffer_state->_buffer);
  text_node_data->original_text.data = buffer_state->_start_original_text;
  text_node_data->original_text.length = state->_current_token->original_text.data - buffer_state->_start_original_text;
  text_node_data->start_pos = buffer_state->_start_position;
  if (state->_foster_parent_insertions && node_tag_in(get_current_node(parser), TAG_TABLE, TAG_TBODY, TAG_TFOOT, TAG_THEAD, TAG_TR, TAG_LAST)) {
    foster_parent_element(parser, text_node);
  } else {
    append_node(parser, parser->_output->root ? get_current_node(parser) : parser->_output->document, text_node);
  }
  debug("Flushing text node buffer of %.*s.\n", (int) buffer_state->_buffer.length, buffer_state->_buffer.data);

  string_buffer_destroy(parser, &buffer_state->_buffer);
  string_buffer_init(parser, &buffer_state->_buffer);
  buffer_state->_type = NODE_WHITESPACE;
  assert(buffer_state->_buffer.length == 0);
}

static void record_end_of_element(Token* current_token, Element* element) {
  element->end_pos = current_token->position;
  element->original_end_tag = current_token->type == TOKEN_END_TAG ? current_token->original_text : kEmptyString;
}

static Node* pop_current_node(Parser* parser) {
  ParserState* state = parser->_parser_state;
  maybe_flush_text_node_buffer(parser);
  if (state->_open_elements.length > 0) {
    assert(node_tag_is((const Node*)state->_open_elements.data[0], TAG_HTML));
    debug("Popping %s node.\n", normalized_tagname(get_current_node(parser)->v.element.tag));
  }
  Node* current_node = (Node*)vector_pop(parser, &state->_open_elements);
  if (!current_node) {
    assert(state->_open_elements.length == 0);
    return NULL;
  }
  assert(current_node->type == NODE_ELEMENT);
  bool is_closed_body_or_html_tag = (node_tag_is(current_node, TAG_BODY) && state->_closed_body_tag) || (node_tag_is(current_node, TAG_HTML) && state->_closed_html_tag);
  if ((state->_current_token->type != TOKEN_END_TAG || !node_tag_is(current_node, state->_current_token->v.end_tag)) && !is_closed_body_or_html_tag) {
    current_node->parse_flags = INSERTION_IMPLICIT_END_TAG;
  }
  if (!is_closed_body_or_html_tag) {
    record_end_of_element(state->_current_token, &current_node->v.element);
  }
  return current_node;
}

static void append_comment_node(Parser* parser, Node* node, const Token* token) {
  maybe_flush_text_node_buffer(parser);
  Node* comment = create_node(parser, NODE_COMMENT);
  comment->type = NODE_COMMENT;
  comment->parse_flags = INSERTION_NORMAL;
  comment->v.text.text = token->v.text;
  comment->v.text.original_text = token->original_text;
  comment->v.text.start_pos = token->position;
  append_node(parser, node, comment);
}

static void clear_stack_to_table_row_context(Parser* parser) {
  while (!node_tag_in(get_current_node(parser), TAG_HTML, TAG_TR, TAG_LAST)) {
    pop_current_node(parser);
  }
}

static void clear_stack_to_table_context(Parser* parser) {
  while (!node_tag_in(get_current_node(parser), TAG_HTML, TAG_TABLE, TAG_LAST)) {
    pop_current_node(parser);
  }
}

void clear_stack_to_table_body_context(Parser* parser) {
  while (!node_tag_in(get_current_node(parser), TAG_HTML, TAG_TBODY, TAG_TFOOT, TAG_THEAD, TAG_LAST)) {
    pop_current_node(parser);
  }
}

// Creates a parser-inserted element in the HTML namespace and returns it.
static Node* create_element(Parser* parser, HTMLTag tag) {
  Node* node = create_node(parser, NODE_ELEMENT);
  Element* element = &node->v.element;
  vector_init(parser, 1, &element->children);
  vector_init(parser, 0, &element->attributes);
  element->tag = tag;
  element->tag_namespace = NAMESPACE_HTML;
  element->original_tag = kEmptyString;
  element->original_end_tag = kEmptyString;
  element->start_pos = parser->_parser_state->_current_token->position;
  element->end_pos = kEmptySrcPosition;
  return node;
}

// Constructs an element from the given start tag token.
static Node* create_element_from_token(Parser* parser, Token* token, NameSpaceEnum tag_namespace) {
  assert(token->type == TOKEN_START_TAG);
  TokenStartTag* start_tag = &token->v.start_tag;

  Node* node = create_node(parser, NODE_ELEMENT);
  Element* element = &node->v.element;
  vector_init(parser, 1, &element->children);
  element->attributes = start_tag->attributes;
  element->tag = start_tag->tag;
  element->tag_namespace = tag_namespace;

  assert(token->original_text.length >= 2);
  assert(token->original_text.data[0] == '<');
  assert(token->original_text.data[token->original_text.length - 1] == '>');
  element->original_tag = token->original_text;
  element->start_pos = token->position;
  element->original_end_tag = kEmptyString;
  element->end_pos = kEmptySrcPosition;

  // The element takes ownership of the attributes from the token, so any
  // allocated-memory fields should be nulled out.
  start_tag->attributes = kEmptyVector;
  return node;
}

static void insert_element(Parser* parser, Node* node, bool is_reconstructing_formatting_elements) {
  ParserState* state = parser->_parser_state;
  if (!is_reconstructing_formatting_elements) {
    maybe_flush_text_node_buffer(parser);
  }
  if (state->_foster_parent_insertions && node_tag_in(get_current_node(parser), TAG_TABLE, TAG_TBODY, TAG_TFOOT, TAG_THEAD, TAG_TR, TAG_LAST)) {
    foster_parent_element(parser, node);
    vector_add(parser, (void*) node, &state->_open_elements);
    return;
  }

  append_node(parser, parser->_output->root ? get_current_node(parser) : parser->_output->document, node);
  vector_add(parser, (void*) node, &state->_open_elements);
}

static Node* insert_element_from_token(Parser* parser, Token* token) {
  Node* element = create_element_from_token(parser, token, NAMESPACE_HTML);
  insert_element(parser, element, false);
  debug("Inserting <%s> element (@%x) from token.\n", normalized_tagname(element->v.element.tag), element);
  return element;
}

static Node* insert_element_of_tag_type(Parser* parser, HTMLTag tag, ParseFlags reason) {
  Node* element = create_element(parser, tag);
  element->parse_flags = (ParseFlags)(INSERTION_BY_PARSER | reason);
  insert_element(parser, element, false);
  debug("Inserting %s element (@%x) from tag type.\n", normalized_tagname(tag), element);
  return element;
}

static Node* insert_foreign_element(Parser* parser, Token* token, NameSpaceEnum tag_namespace) {
  assert(token->type == TOKEN_START_TAG);
  Node* element = create_element_from_token(parser, token, tag_namespace);
  insert_element(parser, element, false);
  if (token_has_attribute(token, "xmlns") && !attribute_matches_case_sensitive(&token->v.start_tag.attributes, "xmlns", kLegalXmlns[tag_namespace])){
    add_parse_error(parser, token);
  }
  if (token_has_attribute(token, "xmlns:xlink") && !attribute_matches_case_sensitive(&token->v.start_tag.attributes, "xmlns:xlink", "http://www.w3.org/1999/xlink")) {
    add_parse_error(parser, token);
  }
  return element;
}

static void insert_text_token(Parser* parser, Token* token) {
  assert(token->type == TOKEN_WHITESPACE || token->type == TOKEN_CHARACTER);
  TextNodeBufferState* buffer_state = &parser->_parser_state->_text_node;
  if (buffer_state->_buffer.length == 0) {
    // Initialize position fields.
    buffer_state->_start_original_text = token->original_text.data;
    buffer_state->_start_position = token->position;
  }
  string_buffer_append_codepoint(parser, token->v.character, &buffer_state->_buffer);
  if (token->type == TOKEN_CHARACTER) {
    buffer_state->_type = NODE_TEXT;
  }
  debug("Inserting text token '%c'.\n", token->v.character);
}

static void run_generic_parsing_algorithm(Parser* parser, Token* token, TokenizerEnum lexer_state) {
  insert_element_from_token(parser, token);
  tokenizer_set_state(parser, lexer_state);
  parser->_parser_state->_original_insertion_mode = parser->_parser_state->_insertion_mode;
  parser->_parser_state->_insertion_mode = INSERTION_MODE_TEXT;
}

static void acknowledge_self_closing_tag(Parser* parser) {
  parser->_parser_state->_self_closing_flag_acknowledged = true;
}

static bool find_last_anchor_index(Parser* parser, int* anchor_index) {
  Vector* elements = &parser->_parser_state->_active_formatting_elements;
  for (int i = elements->length; --i >= 0; ) {
    Node* node = (Node*)elements->data[i];
    if (node == &kActiveFormattingScopeMarker) {
      return false;
    }
    if (node_tag_is(node, TAG_A)) {
      *anchor_index = i;
      return true;
    }
  }
  return false;
}

static int count_formatting_elements_of_tag(Parser* parser, const Node* desired_node, int* earliest_matching_index) {
  const Element* desired_element = &desired_node->v.element;
  Vector* elements = &parser->_parser_state->_active_formatting_elements;
  int num_identical_elements = 0;
  for (int i = elements->length; --i >= 0; ) {
    Node* node = (Node*)elements->data[i];
    if (node == &kActiveFormattingScopeMarker) {
      break;
    }
    assert(node->type == NODE_ELEMENT);
    Element* element = &node->v.element;
    if (node_tag_is(node, desired_element->tag) && element->tag_namespace == desired_element->tag_namespace && all_attributes_match(&element->attributes, &desired_element->attributes)) {
      num_identical_elements++;
      *earliest_matching_index = i;
    }
  }
  return num_identical_elements;
}

static void add_formatting_element(Parser* parser, const Node* node) {
  assert(node == &kActiveFormattingScopeMarker || node->type == NODE_ELEMENT);
  Vector* elements = &parser->_parser_state->_active_formatting_elements;
  if (node == &kActiveFormattingScopeMarker) {
    debug("Adding a scope marker.\n");
  } else {
    debug("Adding a formatting element.\n");
  }

  // Hunt for identical elements.
  int earliest_identical_element = elements->length;
  int num_identical_elements = count_formatting_elements_of_tag(parser, node, &earliest_identical_element);

  // Noah's Ark clause: if there're at least 3, remove the earliest.
  if (num_identical_elements >= 3) {
    debug("Noah's ark clause: removing element at %d.\n", earliest_identical_element);
    vector_remove_at(parser, earliest_identical_element, elements);
  }

  vector_add(parser, (void*) node, elements);
}

static bool is_open_element(Parser* parser, const Node* node) {
  Vector* open_elements = &parser->_parser_state->_open_elements;
  for (int i = 0; i < open_elements->length; ++i) {
    if (open_elements->data[i] == node) {
      return true;
    }
  }
  return false;
}

Node* clone_node(Parser* parser, const Node* node, ParseFlags reason) {
  assert(node->type == NODE_ELEMENT);
  Node* new_node = (Node*)parser_allocate(parser, sizeof(Node));
  *new_node = *node;
  new_node->parent = NULL;
  new_node->index_within_parent = -1;
  // Clear the INSERTION_IMPLICIT_END_TAG flag, as the cloned node may
  // have a separate end tag.
  new_node->parse_flags = (ParseFlags)(~INSERTION_IMPLICIT_END_TAG);// Removido &
  new_node->parse_flags = (ParseFlags)(reason | INSERTION_BY_PARSER);
  Element* element = &new_node->v.element;
  vector_init(parser, 1, &element->children);

  const Vector* old_attributes = &node->v.element.attributes;
  vector_init(parser, old_attributes->length, &element->attributes);
  for (int i = 0; i < old_attributes->length; ++i) {
    const Attribute* old_attr = (const Attribute*)old_attributes->data[i];
    Attribute* attr = (Attribute*)parser_allocate(parser, sizeof(Attribute));
    *attr = *old_attr;
    attr->name = copy_stringz(parser, old_attr->name);
    attr->value = copy_stringz(parser, old_attr->value);
    vector_add(parser, attr, &element->attributes);
  }
  return new_node;
}

static void reconstruct_active_formatting_elements(Parser* parser) {
  Vector* elements = &parser->_parser_state->_active_formatting_elements;
  // Step 1
  if (elements->length == 0) {
    return;
  }

  // Step 2 & 3
  int i = elements->length - 1;
  const Node* element = (const Node*)elements->data[i];
  if (element == &kActiveFormattingScopeMarker || is_open_element(parser, element)) {
    return;
  }

  // Step 6
  do {
    if (i == 0) {
      // Step 4
      i = -1;   // Incremented to 0 below.
      break;
    }
    // Step 5
    element = (const Node*)elements->data[--i];
  } while (element != &kActiveFormattingScopeMarker && !is_open_element(parser, element));

  ++i;
  debug("Reconstructing elements from %d on %s parent.\n", i, normalized_tagname(get_current_node(parser)->v.element.tag));
  for(; i < elements->length; ++i) {
    // Step 7 & 8.
    assert(elements->length > 0);
    assert(i < elements->length);
    element = (const Node*)elements->data[i];
    assert(element != &kActiveFormattingScopeMarker);
    Node* clone = clone_node(
        parser, element, INSERTION_RECONSTRUCTED_FORMATTING_ELEMENT);
    // Step 9.
    insert_element(parser, clone, true);
    // Step 10.
    elements->data[i] = clone;
    debug("Reconstructed %s element at %d.\n", normalized_tagname(clone->v.element.tag), i);
  }
}

static void clear_active_formatting_elements(Parser* parser) {
  Vector* elements = &parser->_parser_state->_active_formatting_elements;
  int num_elements_cleared = 0;
  const Node* node;
  do {
    node = (const Node*)vector_pop(parser, elements);
    ++num_elements_cleared;
  } while(node && node != &kActiveFormattingScopeMarker);
  debug("Cleared %d elements from active formatting list.\n", num_elements_cleared);
}

static QuirksModeEnum compute_quirks_mode(const TokenDocType* doctype) {
  if (doctype->force_quirks || strcmp(doctype->name, kDoctypeHtml.data) || is_in_static_list(doctype->public_identifier, kQuirksModePublicIdPrefixes, false) ||is_in_static_list(doctype->public_identifier,kQuirksModePublicIdExactMatches, true) ||
       is_in_static_list(doctype->system_identifier, kQuirksModeSystemIdExactMatches, true) || (is_in_static_list(doctype->public_identifier, kLimitedQuirksRequiresSystemIdPublicIdPrefixes, false) && !doctype->has_system_identifier)) {
    return DOCTYPE_QUIRKS;
  } else if (is_in_static_list(doctype->public_identifier, kLimitedQuirksPublicIdPrefixes, false) || (is_in_static_list(doctype->public_identifier,kLimitedQuirksRequiresSystemIdPublicIdPrefixes, false) && doctype->has_system_identifier)) {
    return DOCTYPE_LIMITED_QUIRKS;
  }
  return DOCTYPE_NO_QUIRKS;
}

static bool has_an_element_in_specific_scope(Parser* parser, Vector* /* HTMLTag */ expected, bool negate, ...) {
  Vector* open_elements = &parser->_parser_state->_open_elements;
  va_list args;
  va_start(args, negate);
  Vector tags;
  vector_init(parser, 10, &tags);
  for (HTMLTag tag = va_arg(args, HTMLTag); tag != TAG_LAST; tag = va_arg(args, HTMLTag)) {
    // We store the tags inline instead of storing pointers to them.
    vector_add(parser, (void*) tag, &tags);
  }
  va_end(args);

  bool result = false;
  for (int i = open_elements->length; --i >= 0; ) {
    const Node* node = (const Node*)open_elements->data[i];
    if (node->type != NODE_ELEMENT) {
      continue;
    }
    HTMLTag* node_tag = (HTMLTag*)node->v.element.tag;
    for (int j = 0; j < expected->length; ++j) {
      HTMLTag* expected_tag = (HTMLTag*)expected->data[j];
      if (node_tag == expected_tag) {
        result = true;
        goto cleanup;
      }
    }

    bool found_tag = false;
    for (int j = 0; j < tags.length; ++j) {
      HTMLTag* tag = (HTMLTag*)tags.data[j];
      if (tag == node_tag) {
        found_tag = true;
        break;
      }
    }
    if (negate != found_tag) {
      result = false;
      goto cleanup;
    }
  }
cleanup:
  vector_destroy(parser, &tags);
  return result;
}

#define DECLARE_ONE_ELEMENT_VECTOR(varname, from_var) \
    void* varname ## _tmp_array[1] = { (void*) from_var }; \
    Vector varname = { varname ## _tmp_array, 1, 1 }

static bool has_an_element_in_scope(Parser* parser, HTMLTag tag) {
  DECLARE_ONE_ELEMENT_VECTOR(tags, tag);
  return has_an_element_in_specific_scope(parser, &tags, false, TAG_APPLET, TAG_CAPTION, TAG_HTML, TAG_TABLE, TAG_TD, TAG_TH, TAG_MARQUEE, TAG_OBJECT, TAG_MI, TAG_MO, TAG_MN, TAG_MS, TAG_MTEXT, TAG_ANNOTATION_XML, TAG_FOREIGNOBJECT,
                                          TAG_DESC, TAG_TITLE, TAG_LAST);
}

static bool has_node_in_scope(Parser* parser, const Node* node) {
  Vector* open_elements = &parser->_parser_state->_open_elements;
  for (int i = open_elements->length; --i >= 0; ) {
    const Node* current = (const Node*)open_elements->data[i];
    if (current == node) {
      return true;
    }
    if (current->type != NODE_ELEMENT) {
      continue;
    }
    if (node_tag_in(current, TAG_APPLET, TAG_CAPTION, TAG_HTML, TAG_TABLE, TAG_TD, TAG_TH, TAG_MARQUEE, TAG_OBJECT, TAG_MI, TAG_MO, TAG_MN, TAG_MS, TAG_MTEXT, TAG_ANNOTATION_XML, TAG_FOREIGNOBJECT, TAG_DESC, TAG_TITLE, TAG_LAST)) {
      return false;
    }
  }
  assert(false);
  return false;
}

static bool has_an_element_in_scope_with_tagname(Parser* parser, ...) {
  Vector tags;
  // 6 = arbitrary initial size for vector, chosen because the major use-case
  // for this method is heading tags, of which there are 6.
  vector_init(parser, 6, &tags);
  va_list args;
  va_start(args, parser);
  for (HTMLTag tag = va_arg(args, HTMLTag); tag != TAG_LAST; tag = va_arg(args, HTMLTag)) {
    vector_add(parser, (void*) tag, &tags);
  }
  bool found = has_an_element_in_specific_scope(parser, &tags, false, TAG_APPLET, TAG_CAPTION, TAG_HTML, TAG_TABLE, TAG_TD, TAG_TH, TAG_MARQUEE, TAG_OBJECT, TAG_MI, TAG_MO, TAG_MN, TAG_MS, TAG_MTEXT, TAG_ANNOTATION_XML, TAG_FOREIGNOBJECT, TAG_DESC,
                                                TAG_TITLE, TAG_LAST);
  vector_destroy(parser, &tags);
  va_end(args);
  return found;
}

static bool has_an_element_in_list_scope(Parser* parser, HTMLTag tag) {
  DECLARE_ONE_ELEMENT_VECTOR(tags, tag);
  return has_an_element_in_specific_scope(parser, &tags, false, TAG_APPLET, TAG_CAPTION, TAG_HTML, TAG_TABLE, TAG_TD, TAG_TH, TAG_MARQUEE, TAG_OBJECT, TAG_MI, TAG_MO, TAG_MN, TAG_MS, TAG_MTEXT, TAG_ANNOTATION_XML, TAG_FOREIGNOBJECT, TAG_DESC,
                                          TAG_TITLE, TAG_OL, TAG_UL, TAG_LAST);
}

static bool has_an_element_in_button_scope(Parser* parser, HTMLTag tag) {
  DECLARE_ONE_ELEMENT_VECTOR(tags, tag);
  return has_an_element_in_specific_scope(parser, &tags, false, TAG_APPLET, TAG_CAPTION, TAG_HTML, TAG_TABLE, TAG_TD, TAG_TH, TAG_MARQUEE, TAG_OBJECT, TAG_MI, TAG_MO, TAG_MN, TAG_MS, TAG_MTEXT, TAG_ANNOTATION_XML, TAG_FOREIGNOBJECT,TAG_DESC,
                                          TAG_TITLE, TAG_BUTTON, TAG_LAST);
}

static bool has_an_element_in_table_scope(Parser* parser, HTMLTag tag) {
  DECLARE_ONE_ELEMENT_VECTOR(tags, tag);
  return has_an_element_in_specific_scope(parser, &tags, false, TAG_HTML, TAG_TABLE, TAG_LAST);
}

static bool has_an_element_in_select_scope(Parser* parser, HTMLTag tag) {
  DECLARE_ONE_ELEMENT_VECTOR(tags, tag);
  return has_an_element_in_specific_scope(parser, &tags, true, TAG_OPTGROUP, TAG_OPTION, TAG_LAST);
}

static void generate_implied_end_tags(Parser* parser, HTMLTag exception) {
  for (;node_tag_in(get_current_node(parser), TAG_DD, TAG_DT, TAG_LI, TAG_OPTION, TAG_OPTGROUP, TAG_P, TAG_RP, TAG_RT, TAG_LAST) && !node_tag_is(get_current_node(parser), exception); pop_current_node(parser));
}

static bool close_table(Parser* parser) {
  if (!has_an_element_in_table_scope(parser, TAG_TABLE)) {
    return false;
  }

  Node* node = pop_current_node(parser);
  while (!node_tag_is(node, TAG_TABLE)) {
    node = pop_current_node(parser);
  }
  reset_insertion_mode_appropriately(parser);
  return true;
}

static bool close_table_cell(Parser* parser, const Token* token, HTMLTag cell_tag) {
  bool result = true;
  generate_implied_end_tags(parser, TAG_LAST);
  const Node* node = get_current_node(parser);
  if (!node_tag_is(node, cell_tag)) {
    add_parse_error(parser, token);
    result = false;
  }
  do {
    node = pop_current_node(parser);
  } while (!node_tag_is(node, cell_tag));

  clear_active_formatting_elements(parser);
  set_insertion_mode(parser, INSERTION_MODE_IN_ROW);
  return result;
}

static bool close_current_cell(Parser* parser, const Token* token) {
  if (has_an_element_in_table_scope(parser, TAG_TD)) {
    assert(!has_an_element_in_table_scope(parser, TAG_TH));
    return close_table_cell(parser, token, TAG_TD);
  } else {
    assert(has_an_element_in_table_scope(parser, TAG_TH));
    return close_table_cell(parser, token, TAG_TH);
  }
}

static void close_current_select(Parser* parser) {
  Node* node = pop_current_node(parser);
  while (!node_tag_is(node, TAG_SELECT)) {
    node = pop_current_node(parser);
  }
  reset_insertion_mode_appropriately(parser);
}

static bool is_special_node(const Node* node) {
  assert(node->type == NODE_ELEMENT);
  switch (node->v.element.tag_namespace) {
    case NAMESPACE_HTML:
      return node_tag_in(node, TAG_ADDRESS, TAG_APPLET, TAG_AREA, TAG_ARTICLE, TAG_ASIDE, TAG_BASE, TAG_BASEFONT, TAG_BGSOUND, TAG_BLOCKQUOTE, TAG_BODY, TAG_BR, TAG_BUTTON, TAG_CAPTION, TAG_CENTER, TAG_COL, TAG_COLGROUP, TAG_COMMAND, TAG_DD, TAG_DETAILS,
                         TAG_DIR, TAG_DIV, TAG_DL, TAG_DT, TAG_EMBED, TAG_FIELDSET, TAG_FIGCAPTION, TAG_FIGURE,TAG_FOOTER, TAG_FORM, TAG_FRAME, TAG_FRAMESET, TAG_H1, TAG_H2, TAG_H3, TAG_H4, TAG_H5, TAG_H6, TAG_HEAD, TAG_HEADER, TAG_HGROUP, TAG_HR,
                         TAG_HTML, TAG_IFRAME, TAG_IMG, TAG_INPUT, TAG_ISINDEX, TAG_LI, TAG_LINK, TAG_LISTING, TAG_MARQUEE, TAG_MENU, TAG_META, TAG_NAV, TAG_NOEMBED, TAG_NOFRAMES, TAG_NOSCRIPT, TAG_OBJECT, TAG_OL, TAG_P, TAG_PARAM, TAG_PLAINTEXT, TAG_PRE,
                         TAG_SCRIPT, TAG_SECTION, TAG_SELECT, TAG_STYLE, TAG_SUMMARY, TAG_TABLE, TAG_TBODY, TAG_TD, TAG_TEXTAREA, TAG_TFOOT, TAG_TH, TAG_THEAD, TAG_TITLE, TAG_TR, TAG_UL, TAG_WBR, TAG_XMP, TAG_LAST);
    case NAMESPACE_MATHML:
      return node_tag_in(node, TAG_MI, TAG_MO, TAG_MN, TAG_MS, TAG_MTEXT, TAG_ANNOTATION_XML, TAG_LAST);
    case NAMESPACE_SVG:
      return node_tag_in(node, TAG_FOREIGNOBJECT, TAG_DESC, TAG_LAST);
  }
  abort();
  return false;  // Pacify compiler.
}

static bool implicitly_close_tags(Parser* parser, Token* token, HTMLTag target) {
  bool result = true;
  generate_implied_end_tags(parser, target);
  if (!node_tag_is(get_current_node(parser), target)) {
    add_parse_error(parser, token);
    while (!node_tag_is(get_current_node(parser), target)) {
      pop_current_node(parser);
    }
    result = false;
  }
  assert(node_tag_is(get_current_node(parser), target));
  pop_current_node(parser);
  return result;
}

static bool maybe_implicitly_close_p_tag(Parser* parser, Token* token) {
  if (has_an_element_in_button_scope(parser, TAG_P)) {
    return implicitly_close_tags(parser, token, TAG_P);
  }
  return true;
}

static void maybe_implicitly_close_list_tag(Parser* parser, Token* token, bool is_li) {
  ParserState* state = parser->_parser_state;
  state->_frameset_ok = false;
  for (int i = state->_open_elements.length; --i >= 0; ) {
    const Node* node = (const Node*)state->_open_elements.data[i];
    bool is_list_tag = is_li ?
        node_tag_is(node, TAG_LI) :
        node_tag_in(node, TAG_DD, TAG_DT, TAG_LAST);
    if (is_list_tag) {
      implicitly_close_tags(parser, token, node->v.element.tag);
      return;
    }
    if (is_special_node(node) && !node_tag_in(node, TAG_ADDRESS, TAG_DIV, TAG_P, TAG_LAST)) {
      return;
    }
  }
}

static void merge_attributes(Parser* parser, Token* token, Node* node) {
  assert(token->type == TOKEN_START_TAG);
  assert(node->type == NODE_ELEMENT);
  const Vector* token_attr = &token->v.start_tag.attributes;
  Vector* node_attr = &node->v.element.attributes;

  for (int i = 0; i < token_attr->length; ++i) {
    Attribute* attr = (Attribute*)token_attr->data[i];
    if (!get_attribute(node_attr, attr->name)) {
      // Ownership of the attribute is transferred by this gumbo_vector_add,
      // so it has to be nulled out of the original token so it doesn't get
      // double-deleted.
      vector_add(parser, attr, node_attr);
      token_attr->data[i] = NULL;
    }
  }
  // When attributes are merged, it means the token has been ignored and merged
  // with another token, so we need to free its memory.  The attributes that are
  // transferred need to be nulled-out in the vector above so that they aren't
  // double-deleted.
  token_destroy(parser, token);

#ifndef NDEBUG
  // Mark this sentinel so the assertion in the main loop knows it's been
  // destroyed.
  token->v.start_tag.attributes = kEmptyVector;
#endif
}

const char* normalize_svg_tagname(const StringPiece* tag) {
  for (int i = 0; i < sizeof(kSvgTagReplacements) / sizeof(ReplacementEntry); ++i) {
    const ReplacementEntry* entry = &kSvgTagReplacements[i];
    if (string_equals_ignore_case(tag, &entry->from)) {
      return entry->to.data;
    }
  }
  return NULL;
}

static void adjust_foreign_attributes(Parser* parser, Token* token) {
  assert(token->type == TOKEN_START_TAG);
  const Vector* attributes = &token->v.start_tag.attributes;
  for (int i = 0; i < sizeof(kForeignAttributeReplacements) / sizeof(NamespacedAttributeReplacement); ++i) {
    const NamespacedAttributeReplacement* entry = &kForeignAttributeReplacements[i];
    Attribute* attr = get_attribute(attributes, entry->from);
    if (!attr) {
      continue;
    }
    parser_deallocate(parser, (void*) attr->name);
    attr->attr_namespace = entry->attr_namespace;
    attr->name = copy_stringz(parser, entry->local_name);
  }
}

static void adjust_svg_attributes(Parser* parser, Token* token) {
  assert(token->type == TOKEN_START_TAG);
  const Vector* attributes = &token->v.start_tag.attributes;
  for (int i = 0; i < sizeof(kSvgAttributeReplacements) / sizeof(ReplacementEntry); ++i) {
    const ReplacementEntry* entry = &kSvgAttributeReplacements[i];
    Attribute* attr = get_attribute(attributes, entry->from.data);
    if (!attr) {
      continue;
    }
    parser_deallocate(parser, (void*) attr->name);
    attr->name = copy_stringz(parser, entry->to.data);
  }
}

static void adjust_mathml_attributes(Parser* parser, Token* token) {
  assert(token->type == TOKEN_START_TAG);
  Attribute* attr = get_attribute(&token->v.start_tag.attributes, "definitionurl");
  if (!attr) {
    return;
  }
  parser_deallocate(parser, (void*) attr->name);
  attr->name = copy_stringz(parser, "definitionURL");
}

static bool doctype_matches( const TokenDocType* doctype, const StringPiece* public_id, const StringPiece* system_id, bool allow_missing_system_id) {
  return !strcmp(doctype->public_identifier, public_id->data) && (allow_missing_system_id || doctype->has_system_identifier) && !strcmp(doctype->system_identifier, system_id->data);
}

static bool maybe_add_doctype_error(Parser* parser, const Token* token) {
  const TokenDocType* doctype = &token->v.doc_type;
  bool html_doctype = !strcmp(doctype->name, kDoctypeHtml.data);
  if (!html_doctype || doctype->has_public_identifier || (doctype->has_system_identifier && !strcmp(doctype->system_identifier, kSystemIdLegacyCompat.data)) ||
       !(html_doctype && (doctype_matches(doctype, &kPublicIdHtml4_0, &kSystemIdRecHtml4_0, true) || doctype_matches(doctype, &kPublicIdHtml4_01, &kSystemIdHtml4, true) || doctype_matches(doctype, &kPublicIdXhtml1_0, &kSystemIdXhtmlStrict1_1, false) ||
                           doctype_matches(doctype, &kPublicIdXhtml1_1, &kSystemIdXhtml1_1, false)))) {
    add_parse_error(parser, token);
    return false;
  }
  return true;
}

static void remove_from_parent(Parser* parser, Node* node) {
  if (!node->parent) {
    // The node may not have a parent if, for example, it is a newly-cloned copy
    // of an active formatting element.  DOM manipulations continue with the
    // orphaned fragment of the DOM tree until it's appended/foster-parented to
    // the common ancestor at the end of the adoption agency algorithm.
    return;
  }
  assert(node->parent->type == NODE_ELEMENT);
  Vector* children = &node->parent->v.element.children;
  int index = vector_index_of(children, node);
  assert(index != -1);

  vector_remove_at(parser, index, children);
  node->parent = NULL;
  node->index_within_parent = -1;
  for (int i = index; i < children->length; ++i) {
    Node* child = (Node*)children->data[i];
    child->index_within_parent = i;
  }
}

static bool adoption_agency_algorithm(Parser* parser, Token* token, HTMLTag closing_tag) {
  ParserState* state = parser->_parser_state;
  debug("Entering adoption agency algorithm.\n");
  // Steps 1-3 & 16:
  for (int i = 0; i < 8; ++i) {
    // Step 4.
    Node* formatting_node = NULL;
    int formatting_node_in_open_elements = -1;
    for (int j = state->_active_formatting_elements.length; --j >= 0; ) {
      Node* current_node = (Node*)state->_active_formatting_elements.data[j];
      if (current_node == &kActiveFormattingScopeMarker) {
        debug("Broke on scope marker; aborting.\n");
        // Last scope marker; abort the algorithm.
        return false;
      }
      if (node_tag_is(current_node, closing_tag)) {
        // Found it.
        formatting_node = current_node;
        formatting_node_in_open_elements = vector_index_of(&state->_open_elements, formatting_node);
        debug("Formatting element of tag %s at %d.\n", normalized_tagname(closing_tag), formatting_node_in_open_elements);
        break;
      }
    }
    if (!formatting_node) {
      // No matching tag; not a parse error outright, but fall through to the
      // "any other end tag" clause (which may potentially add a parse error,
      // but not always).
      debug("No active formatting elements; aborting.\n");
      return false;
    }

    if (formatting_node_in_open_elements == -1) {
      debug("Formatting node not on stack of open elements.\n");
      vector_remove(parser, formatting_node, &state->_active_formatting_elements);
      return false;
    }

    if (!has_an_element_in_scope(parser, formatting_node->v.element.tag)) {
      add_parse_error(parser, token);
      debug("Element not in scope.\n");
      return false;
    }
    if (formatting_node != get_current_node(parser)) {
      add_parse_error(parser, token);  // But continue onwards.
    }
    assert(formatting_node);
    assert(!node_tag_is(formatting_node, TAG_HTML));
    assert(!node_tag_is(formatting_node, TAG_BODY));

    // Step 5 & 6.
    Node* furthest_block = NULL;
    for (int j = formatting_node_in_open_elements; j < state->_open_elements.length; ++j) {
      assert(j > 0);
      Node* current = (Node*)state->_open_elements.data[j];
      if (is_special_node(current)) {
        // Step 5.
        furthest_block = current;
        break;
      }
    }
    if (!furthest_block) {
      // Step 6.
      while (get_current_node(parser) != formatting_node) {
        pop_current_node(parser);
      }
      // And the formatting element itself.
      pop_current_node(parser);
      vector_remove(parser, formatting_node, &state->_active_formatting_elements);
      return false;
    }
    assert(!node_tag_is(furthest_block, TAG_HTML));
    assert(furthest_block);

    // Step 7.
    // Elements may be moved and reparented by this algorithm, so
    // common_ancestor is not necessarily the same as formatting_node->parent.
    Node* common_ancestor = (Node*)state->_open_elements.data[vector_index_of(&state->_open_elements, formatting_node) - 1];
    debug("Common ancestor tag = %s, furthest block tag = %s.\n", normalized_tagname(common_ancestor->v.element.tag), normalized_tagname(furthest_block->v.element.tag));

    // Step 8.
    int bookmark = vector_index_of(&state->_active_formatting_elements, formatting_node);;
    // Step 9.
    Node* node = furthest_block;
    Node* last_node = furthest_block;
    // Must be stored explicitly, in case node is removed from the stack of open
    // elements, to handle step 9.4.
    int saved_node_index = vector_index_of(&state->_open_elements, node);
    assert(saved_node_index > 0);
    // Step 9.1-9.3 & 9.11.
    for (int j = 0; j < 3; ++j) {
      // Step 9.4.
      int node_index = vector_index_of(&state->_open_elements, node);
      debug("Current index: %d, last index: %d.\n", node_index, saved_node_index);
      if (node_index == -1) {
        node_index = saved_node_index;
      }
      saved_node_index = --node_index;
      assert(node_index > 0);
      assert(node_index < state->_open_elements.capacity);
      node = (Node*)state->_open_elements.data[node_index];
      assert(node->parent);
      // Step 9.5.
      if (vector_index_of(&state->_active_formatting_elements, node) == -1) {
        vector_remove_at(parser, node_index, &state->_open_elements);
        continue;
      } else if (node == formatting_node) {
        // Step 9.6.
        break;
      }
      // Step 9.7.
      int formatting_index = vector_index_of(&state->_active_formatting_elements, node);
      node = clone_node(parser, node, INSERTION_ADOPTION_AGENCY_CLONED);
      state->_active_formatting_elements.data[formatting_index] = node;
      state->_open_elements.data[node_index] = node;
      // Step 9.8.
      if (last_node == furthest_block) {
        bookmark = formatting_index + 1;
        assert(bookmark <= state->_active_formatting_elements.length);
      }
      // Step 9.9.
      last_node->parse_flags = INSERTION_ADOPTION_AGENCY_MOVED;
      remove_from_parent(parser, last_node);
      append_node(parser, node, last_node);
      // Step 9.10.
      last_node = node;
    }

    // Step 10.
    debug("Removing %s node from parent ", normalized_tagname(last_node->v.element.tag));
    remove_from_parent(parser, last_node);
    last_node->parse_flags = INSERTION_ADOPTION_AGENCY_MOVED;
    if (node_tag_in(common_ancestor, TAG_TABLE, TAG_TBODY, TAG_TFOOT, TAG_THEAD, TAG_TR, TAG_LAST)) {
      debug("and foster-parenting it.\n");
      foster_parent_element(parser, last_node);
    } else {
      debug("and inserting it into %s.\n", normalized_tagname(common_ancestor->v.element.tag));
      append_node(parser, common_ancestor, last_node);
    }

    // Step 11.
    Node* new_formatting_node = clone_node(parser, formatting_node, INSERTION_ADOPTION_AGENCY_CLONED);
    formatting_node->parse_flags = INSERTION_IMPLICIT_END_TAG;

    // Step 12.  Instead of appending nodes one-by-one, we swap the children
    // vector of furthest_block with the empty children of new_formatting_node,
    // reducing memory traffic and allocations.  We still have to reset their
    // parent pointers, though.
    Vector temp = new_formatting_node->v.element.children;
    new_formatting_node->v.element.children = furthest_block->v.element.children;
    furthest_block->v.element.children = temp;

    temp = new_formatting_node->v.element.children;
    for (int i = 0; i < temp.length; ++i) {
      Node* child = (Node*)temp.data[i];
      child->parent = new_formatting_node;
    }

    // Step 13.
    append_node(parser, furthest_block, new_formatting_node);

    // Step 14.
    // If the formatting node was before the bookmark, it may shift over all
    // indices after it, so we need to explicitly find the index and possibly
    // adjust the bookmark.
    int formatting_node_index = vector_index_of(&state->_active_formatting_elements, formatting_node);
    assert(formatting_node_index != -1);
    if (formatting_node_index < bookmark) {
      --bookmark;
    }
    vector_remove_at(parser, formatting_node_index, &state->_active_formatting_elements);
    assert(bookmark >= 0);
    assert(bookmark <= state->_active_formatting_elements.length);
    vector_insert_at(parser, new_formatting_node, bookmark, &state->_active_formatting_elements);

    // Step 15.
    vector_remove(parser, formatting_node, &state->_open_elements);
    int insert_at = vector_index_of(&state->_open_elements, furthest_block) + 1;
    assert(insert_at >= 0);
    assert(insert_at <= state->_open_elements.length);
    vector_insert_at(parser, new_formatting_node, insert_at, &state->_open_elements);
  }
  return true;
}

static void ignore_token(Parser* parser) {
  Token* token = parser->_parser_state->_current_token;
  // Ownership of the token's internal buffers are normally transferred to the
  // element, but if no element is emitted (as happens in non-verbatim-mode
  // when a token is ignored), we need to free it here to prevent a memory
  // leak.
  token_destroy(parser, token);
#ifndef NDEBUG
  if (token->type == TOKEN_START_TAG) {
    // Mark this sentinel so the assertion in the main loop knows it's been
    // destroyed.
    token->v.start_tag.attributes = kEmptyVector;
  }
#endif
}

static void finish_parsing(Parser* parser) {
  maybe_flush_text_node_buffer(parser);
  ParserState* state = parser->_parser_state;
  for (Node* node = pop_current_node(parser); node; node = pop_current_node(parser)) {
    if ((node_tag_is(node, TAG_BODY) && state->_closed_body_tag) || (node_tag_is(node, TAG_HTML) && state->_closed_html_tag)) {
      continue;
    }
    node->parse_flags = INSERTION_IMPLICIT_END_TAG;
  }
  while (pop_current_node(parser));  // Pop them all.
}

static bool handle_initial(Parser* parser, Token* token) {
  Document* document = &get_document_node(parser)->v.document;
  if (token->type == TOKEN_WHITESPACE) {
    ignore_token(parser);
    return true;
  } else if (token->type == TOKEN_COMMENT) {
    append_comment_node(parser, get_document_node(parser), token);
    return true;
  } else if (token->type == TOKEN_DOCTYPE) {
    document->has_doctype = true;
    document->name = token->v.doc_type.name;
    document->public_identifier = token->v.doc_type.public_identifier;
    document->system_identifier = token->v.doc_type.system_identifier;
    document->doc_type_quirks_mode = compute_quirks_mode(&token->v.doc_type);
    set_insertion_mode(parser, INSERTION_MODE_BEFORE_HTML);
    return maybe_add_doctype_error(parser, token);
  }
  add_parse_error(parser, token);
  document->doc_type_quirks_mode = DOCTYPE_QUIRKS;
  set_insertion_mode(parser, INSERTION_MODE_BEFORE_HTML);
  parser->_parser_state->_reprocess_current_token = true;
  return true;
}

static bool handle_before_html(Parser* parser, Token* token) {
  if (token->type == TOKEN_DOCTYPE) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (token->type == TOKEN_COMMENT) {
    append_comment_node(parser, get_document_node(parser), token);
    return true;
  } else if (token->type == TOKEN_WHITESPACE) {
    ignore_token(parser);
    return true;
  } else if (tag_is(token, kStartTag, TAG_HTML)) {
    Node* html_node = insert_element_from_token(parser, token);
    parser->_output->root = html_node;
    set_insertion_mode(parser, INSERTION_MODE_BEFORE_HEAD);
    return true;
  } else if (token->type == TOKEN_END_TAG && !tag_in(token, false, TAG_HEAD, TAG_BODY, TAG_HTML, TAG_BR, TAG_LAST)) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else {
    Node* html_node = insert_element_of_tag_type(parser, TAG_HTML, INSERTION_IMPLIED);
    assert(html_node);
    parser->_output->root = html_node;
    set_insertion_mode(parser, INSERTION_MODE_BEFORE_HEAD);
    parser->_parser_state->_reprocess_current_token = true;
    return true;
  }
}

static bool handle_before_head(Parser* parser, Token* token) {
  if (token->type == TOKEN_DOCTYPE) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (token->type == TOKEN_COMMENT) {
    append_comment_node(parser, get_current_node(parser), token);
    return true;
  } else if (token->type == TOKEN_WHITESPACE) {
    ignore_token(parser);
    return true;
  } else if (tag_is(token, kStartTag, TAG_HEAD)) {
    Node* node = insert_element_from_token(parser, token);
    set_insertion_mode(parser, INSERTION_MODE_IN_HEAD);
    parser->_parser_state->_head_element = node;
    return true;
  } else if (token->type == TOKEN_END_TAG && !tag_in(token, false, TAG_HEAD, TAG_BODY, TAG_HTML,TAG_BR, TAG_LAST)) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else {
    Node* node = insert_element_of_tag_type(parser, TAG_HEAD, INSERTION_IMPLIED);
    set_insertion_mode(parser, INSERTION_MODE_IN_HEAD);
    parser->_parser_state->_head_element = node;
    parser->_parser_state->_reprocess_current_token = true;
    return true;
  }
}

// Forward declarations because of mutual dependencies.
static bool handle_token(Parser* parser, Token* token);
static bool handle_in_body(Parser* parser, Token* token);

static bool handle_in_head(Parser* parser, Token* token) {
  if (token->type == TOKEN_WHITESPACE) {
    insert_text_token(parser, token);
    return true;
  } else if (token->type == TOKEN_DOCTYPE) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (token->type == TOKEN_COMMENT) {
    append_comment_node(parser, get_current_node(parser), token);
    return true;
  } else if (tag_is(token, kStartTag, TAG_HTML)) {
    return handle_in_body(parser, token);
  } else if (tag_in(token, kStartTag, TAG_BASE, TAG_BASEFONT, TAG_BGSOUND, TAG_COMMAND, TAG_LINK, TAG_LAST)) {
    insert_element_from_token(parser, token);
    pop_current_node(parser);
    acknowledge_self_closing_tag(parser);
    return true;
  } else if (tag_is(token, kStartTag, TAG_META)) {
    insert_element_from_token(parser, token);
    pop_current_node(parser);
    acknowledge_self_closing_tag(parser);
    // NOTE(jdtang): Gumbo handles only UTF-8, so the encoding clause of the
    // spec doesn't apply.  If clients want to handle meta-tag re-encoding, they
    // should specifically look for that string in the document and re-encode it
    // before passing to Gumbo.
    return true;
  } else if (tag_is(token, kStartTag, TAG_TITLE)) {
    run_generic_parsing_algorithm(parser, token, RCDATA);
    return true;
  } else if (tag_in(token, kStartTag, TAG_NOFRAMES, TAG_STYLE,TAG_LAST)) {
    run_generic_parsing_algorithm(parser, token, RAWTEXT);
    return true;
  } else if (tag_is(token, kStartTag, TAG_NOSCRIPT)) {
    insert_element_from_token(parser, token);
    set_insertion_mode(parser, INSERTION_MODE_IN_HEAD_NOSCRIPT);
    return true;
  } else if (tag_is(token, kStartTag, TAG_SCRIPT)) {
    run_generic_parsing_algorithm(parser, token, SCRIPT);
    return true;
  } else if (tag_is(token, kEndTag, TAG_HEAD)) {
    Node* head = pop_current_node(parser);
    AVOID_UNUSED_VARIABLE_WARNING(head);
    assert(node_tag_is(head, TAG_HEAD));
    set_insertion_mode(parser, INSERTION_MODE_AFTER_HEAD);
    return true;
  } else if (tag_is(token, kStartTag, TAG_HEAD)) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (tag_is(token, kStartTag, TAG_HEAD) || (token->type == TOKEN_END_TAG && !tag_in(token, kEndTag, TAG_BODY, TAG_HTML, TAG_BR, TAG_LAST))) {
    add_parse_error(parser, token);
    return false;
  } else {
    const Node* node = pop_current_node(parser);
    assert(node_tag_is(node, TAG_HEAD));
    AVOID_UNUSED_VARIABLE_WARNING(node);
    set_insertion_mode(parser, INSERTION_MODE_AFTER_HEAD);
    parser->_parser_state->_reprocess_current_token = true;
    return true;
  }

  return true;
}

static bool handle_in_head_noscript(Parser* parser, Token* token) {
  if (token->type == TOKEN_DOCTYPE) {
    add_parse_error(parser, token);
    return false;
  } else if (tag_is(token, kStartTag, TAG_HTML)) {
    return handle_in_body(parser, token);
  } else if (tag_is(token, kEndTag, TAG_NOSCRIPT)) {
    const Node* node = pop_current_node(parser);
    assert(node_tag_is(node, TAG_NOSCRIPT));
    AVOID_UNUSED_VARIABLE_WARNING(node);
    set_insertion_mode(parser, INSERTION_MODE_IN_HEAD);
    return true;
  } else if (token->type == TOKEN_WHITESPACE || token->type == TOKEN_COMMENT || tag_in(token, kStartTag, TAG_BASEFONT, TAG_BGSOUND, TAG_LINK, TAG_META, TAG_NOFRAMES, TAG_STYLE, TAG_LAST)) {
    return handle_in_head(parser, token);
  } else if (tag_in(token, kStartTag, TAG_HEAD, TAG_NOSCRIPT, TAG_LAST) || (token->type == TOKEN_END_TAG && !tag_is(token, kEndTag, TAG_BR))) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else {
    add_parse_error(parser, token);
    const Node* node = pop_current_node(parser);
    assert(node_tag_is(node, TAG_NOSCRIPT));
    AVOID_UNUSED_VARIABLE_WARNING(node);
    set_insertion_mode(parser, INSERTION_MODE_IN_HEAD);
    parser->_parser_state->_reprocess_current_token = true;
    return false;
  }
}

static bool handle_after_head(Parser* parser, Token* token) {
  ParserState* state = parser->_parser_state;
  if (token->type == TOKEN_WHITESPACE) {
    insert_text_token(parser, token);
    return true;
  } else if (token->type == TOKEN_DOCTYPE) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (token->type == TOKEN_COMMENT) {
    append_comment_node(parser, get_current_node(parser), token);
    return true;
  } else if (tag_is(token, kStartTag, TAG_HTML)) {
    return handle_in_body(parser, token);
  } else if (tag_is(token, kStartTag, TAG_BODY)) {
    insert_element_from_token(parser, token);
    state->_frameset_ok = false;
    set_insertion_mode(parser, INSERTION_MODE_IN_BODY);
    return true;
  } else if (tag_is(token, kStartTag, TAG_FRAMESET)) {
    insert_element_from_token(parser, token);
    set_insertion_mode(parser, INSERTION_MODE_IN_FRAMESET);
    return true;
  } else if (tag_in(token, kStartTag, TAG_BASE, TAG_BASEFONT, TAG_BGSOUND, TAG_LINK, TAG_META, TAG_NOFRAMES, TAG_SCRIPT, TAG_STYLE, TAG_TITLE, TAG_LAST)) {
    add_parse_error(parser, token);
    assert(state->_head_element != NULL);
    // This must be flushed before we push the head element on, as there may be
    // pending character tokens that should be attached to the root.
    maybe_flush_text_node_buffer(parser);
    vector_add(parser, state->_head_element, &state->_open_elements);
    bool result = handle_in_head(parser, token);
    vector_remove(parser, state->_head_element, &state->_open_elements);
    return result;
  } else if (tag_is(token, kStartTag, TAG_HEAD) || (token->type == TOKEN_END_TAG && !tag_in(token, kEndTag, TAG_BODY, TAG_HTML, TAG_BR, TAG_LAST))) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else {
    insert_element_of_tag_type(parser, TAG_BODY, INSERTION_IMPLIED);
    set_insertion_mode(parser, INSERTION_MODE_IN_BODY);
    state->_reprocess_current_token = true;
    return true;
  }
}

static void destroy_node(Parser* parser, Node* node) {
  switch (node->type) {
    case NODE_DOCUMENT:
      {
        Document* doc = &node->v.document;
        for (int i = 0; i < doc->children.length; ++i) {
          destroy_node(parser, (Node*)doc->children.data[i]);
        }
        parser_deallocate(parser, (void*) doc->children.data);
        parser_deallocate(parser, (void*) doc->name);
        parser_deallocate(parser, (void*) doc->public_identifier);
        parser_deallocate(parser, (void*) doc->system_identifier);
      }
      break;
    case NODE_ELEMENT:
      for (int i = 0; i < node->v.element.attributes.length; ++i) {
        destroy_attribute(parser, (Attribute*)node->v.element.attributes.data[i]);
      }
      parser_deallocate(parser, node->v.element.attributes.data);
      for (int i = 0; i < node->v.element.children.length; ++i) {
        destroy_node(parser, (Node*)node->v.element.children.data[i]);
      }
      parser_deallocate(parser, node->v.element.children.data);
      break;
    case NODE_TEXT:
    case NODE_CDATA:
    case NODE_COMMENT:
    case NODE_WHITESPACE:
      parser_deallocate(parser, (void*) node->v.text.text);
      break;
  }
  parser_deallocate(parser, node);
}

static bool handle_in_body(Parser* parser, Token* token) {
  ParserState* state = parser->_parser_state;
  assert(state->_open_elements.length > 0);
  if (token->type == TOKEN_NULL) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (token->type == TOKEN_WHITESPACE) {
    reconstruct_active_formatting_elements(parser);
    insert_text_token(parser, token);
    return true;
  } else if (token->type == TOKEN_CHARACTER) {
    reconstruct_active_formatting_elements(parser);
    insert_text_token(parser, token);
    set_frameset_not_ok(parser);
    return true;
  } else if (token->type == TOKEN_COMMENT) {
    append_comment_node(parser, get_current_node(parser), token);
    return true;
  } else if (token->type == TOKEN_DOCTYPE) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (tag_is(token, kStartTag, TAG_HTML)) {
    assert(parser->_output->root != NULL);
    assert(parser->_output->root->type == NODE_ELEMENT);
    add_parse_error(parser, token);
    merge_attributes(parser, token, parser->_output->root);
    return false;
  } else if (tag_in(token, kStartTag, TAG_BASE, TAG_BASEFONT, TAG_BGSOUND, TAG_COMMAND, TAG_LINK, TAG_META, TAG_NOFRAMES, TAG_SCRIPT, TAG_STYLE, TAG_TITLE, TAG_LAST)) {
    return handle_in_head(parser, token);
  } else if (tag_is(token, kStartTag, TAG_BODY)) {
    add_parse_error(parser, token);
    if (state->_open_elements.length < 2 || !node_tag_is((const Node*)state->_open_elements.data[1], TAG_BODY)) {
      ignore_token(parser);
      return false;
    }
    state->_frameset_ok = false;
    merge_attributes(parser, token, (Node*)state->_open_elements.data[1]);
    return false;
  } else if (tag_is(token, kStartTag, TAG_FRAMESET)) {
    add_parse_error(parser, token);
    if (state->_open_elements.length < 2 || !node_tag_is((const Node*)state->_open_elements.data[1], TAG_BODY) || !state->_frameset_ok) {
      ignore_token(parser);
      return false;
    }
    // Save the body node for later removal.
    Node* body_node = (Node*)state->_open_elements.data[1];

    // Pop all nodes except root HTML element.
    Node* node;
    do {
      node = pop_current_node(parser);
    } while (node != state->_open_elements.data[1]);

    // Remove the body node.  We may want to factor this out into a generic
    // helper, but right now this is the only code that needs to do this.
    Vector* children = &parser->_output->root->v.element.children;
    for (int i = 0; i < children->length; ++i) {
      if (children->data[i] == body_node) {
        vector_remove_at(parser, i, children);
        break;
      }
    }
    destroy_node(parser, body_node);

    // Insert the <frameset>, and switch the insertion mode.
    insert_element_from_token(parser, token);
    set_insertion_mode(parser, INSERTION_MODE_IN_FRAMESET);
    return true;
  } else if (token->type == TOKEN_EOF) {
    for (int i = 0; i < state->_open_elements.length; ++i) {
      if (!node_tag_in((const Node*)state->_open_elements.data[i], TAG_DD, TAG_DT, TAG_LI, TAG_P, TAG_TBODY, TAG_TD, TAG_TFOOT, TAG_TH, TAG_THEAD, TAG_TR, TAG_BODY, TAG_HTML, TAG_LAST)) {
        add_parse_error(parser, token);
        return false;
      }
    }
    return true;
  } else if (tag_in(token, kEndTag, TAG_BODY, TAG_HTML, TAG_LAST)) {
    if (!has_an_element_in_scope(parser, TAG_BODY)) {
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    bool success = true;
    for (int i = 0; i < state->_open_elements.length; ++i) {
      if (!node_tag_in((const Node*)state->_open_elements.data[i], TAG_DD, TAG_DT, TAG_LI, TAG_OPTGROUP, TAG_OPTION, TAG_P, TAG_RP, TAG_RT, TAG_TBODY, TAG_TD, TAG_TFOOT, TAG_TH, TAG_THEAD, TAG_TR, TAG_BODY, TAG_HTML, TAG_LAST)) {
        add_parse_error(parser, token);
        success = false;
        break;
      }
    }
    set_insertion_mode(parser, INSERTION_MODE_AFTER_BODY);
    if (tag_is(token, kEndTag, TAG_HTML)) {
      parser->_parser_state->_reprocess_current_token = true;
    } else {
      Node* body = (Node*)state->_open_elements.data[1];
      assert(node_tag_is(body, TAG_BODY));
      record_end_of_element(state->_current_token, &body->v.element);
    }
    return success;
  } else if (tag_in(token, kStartTag, TAG_ADDRESS, TAG_ARTICLE, TAG_ASIDE, TAG_BLOCKQUOTE, TAG_CENTER, TAG_DETAILS, TAG_DIR, TAG_DIV, TAG_DL, TAG_FIELDSET, TAG_FIGCAPTION, TAG_FIGURE, TAG_FOOTER, TAG_HEADER, TAG_HGROUP, TAG_MENU, TAG_NAV, TAG_OL, TAG_P,
                    TAG_SECTION, TAG_SUMMARY, TAG_UL, TAG_LAST)) {
    bool result = maybe_implicitly_close_p_tag(parser, token);
    insert_element_from_token(parser, token);
    return result;
  } else if (tag_in(token, kStartTag, TAG_H1, TAG_H2, TAG_H3, TAG_H4, TAG_H5, TAG_H6, TAG_LAST)) {
    bool result = maybe_implicitly_close_p_tag(parser, token);
    if (node_tag_in(get_current_node(parser), TAG_H1, TAG_H2, TAG_H3, TAG_H4, TAG_H5, TAG_H6, TAG_LAST)) {
      add_parse_error(parser, token);
      pop_current_node(parser);
      result = false;
    }
    insert_element_from_token(parser, token);
    return result;
  } else if (tag_in(token, kStartTag, TAG_PRE, TAG_LISTING, TAG_LAST)) {
    bool result = maybe_implicitly_close_p_tag(parser, token);
    insert_element_from_token(parser, token);
    state->_ignore_next_linefeed = true;
    state->_frameset_ok = false;
    return result;
  } else if (tag_is(token, kStartTag, TAG_FORM)) {
    if (state->_form_element != NULL) {
      debug("Ignoring nested form.\n");
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    bool result = maybe_implicitly_close_p_tag(parser, token);
    state->_form_element = insert_element_from_token(parser, token);
    return result;
  } else if (tag_is(token, kStartTag, TAG_LI)) {
    maybe_implicitly_close_list_tag(parser, token, true);
    bool result = maybe_implicitly_close_p_tag(parser, token);
    insert_element_from_token(parser, token);
    return result;
  } else if (tag_in(token, kStartTag, TAG_DD, TAG_DT, TAG_LAST)) {
    maybe_implicitly_close_list_tag(parser, token, false);
    bool result = maybe_implicitly_close_p_tag(parser, token);
    insert_element_from_token(parser, token);
    return result;
  } else if (tag_is(token, kStartTag, TAG_PLAINTEXT)) {
    bool result = maybe_implicitly_close_p_tag(parser, token);
    insert_element_from_token(parser, token);
    tokenizer_set_state(parser, PLAINTEXT);
    return result;
  } else if (tag_is(token, kStartTag, TAG_BUTTON)) {
    if (has_an_element_in_scope(parser, TAG_BUTTON)) {
      add_parse_error(parser, token);
      implicitly_close_tags(parser, token, TAG_BUTTON);
      state->_reprocess_current_token = true;
      return false;
    }
    reconstruct_active_formatting_elements(parser);
    insert_element_from_token(parser, token);
    state->_frameset_ok = false;
    return true;
  } else if (tag_in(token, kEndTag, TAG_ADDRESS, TAG_ARTICLE, TAG_ASIDE, TAG_BLOCKQUOTE, TAG_BUTTON, TAG_CENTER, TAG_DETAILS, TAG_DIR, TAG_DIV, TAG_DL, TAG_FIELDSET, TAG_FIGCAPTION, TAG_FIGURE, TAG_FOOTER, TAG_HEADER, TAG_HGROUP, TAG_LISTING, TAG_MENU, TAG_NAV,
                    TAG_OL, TAG_PRE, TAG_SECTION, TAG_SUMMARY, TAG_UL, TAG_LAST)) {
    HTMLTag tag = token->v.end_tag;
    if (!has_an_element_in_scope(parser, tag)) {
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    implicitly_close_tags(parser, token, token->v.end_tag);
    return true;
  } else if (tag_is(token, kEndTag, TAG_FORM)) {
    bool result = true;
    const Node* node = state->_form_element;
    assert(!node || node->type == NODE_ELEMENT);
    state->_form_element = NULL;
    if (!node || !has_node_in_scope(parser, node)) {
      debug("Closing an unopened form.\n");
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    // This differs from implicitly_close_tags because we remove *only* the
    // <form> element; other nodes are left in scope.
    generate_implied_end_tags(parser, TAG_LAST);
    if (get_current_node(parser) != node) {
      add_parse_error(parser, token);
      result = false;
    }

    Vector* open_elements = &state->_open_elements;
    int index = open_elements->length - 1;
    for (; index >= 0 && open_elements->data[index] != node; --index); assert(index >= 0);
    vector_remove_at(parser, index, open_elements);
    return result;
  } else if (tag_is(token, kEndTag, TAG_P)) {
    if (!has_an_element_in_button_scope(parser, TAG_P)) {
      add_parse_error(parser, token);
      reconstruct_active_formatting_elements(parser);
      insert_element_of_tag_type(parser, TAG_P, INSERTION_CONVERTED_FROM_END_TAG);
      state->_reprocess_current_token = true;
      return false;
    }
    return implicitly_close_tags(parser, token, TAG_P);
  } else if (tag_is(token, kEndTag, TAG_LI)) {
    if (!has_an_element_in_list_scope(parser, TAG_LI)) {
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    return implicitly_close_tags(parser, token, TAG_LI);
  } else if (tag_in(token, kEndTag, TAG_DD, TAG_DT, TAG_LAST)) {
    assert(token->type == TOKEN_END_TAG);
    HTMLTag token_tag = token->v.end_tag;
    if (!has_an_element_in_scope(parser, token_tag)) {
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    return implicitly_close_tags(parser, token, token_tag);
  } else if (tag_in(token, kEndTag, TAG_H1, TAG_H2, TAG_H3, TAG_H4, TAG_H5, TAG_H6, TAG_LAST)) {
    if (!has_an_element_in_scope_with_tagname(parser, TAG_H1, TAG_H2, TAG_H3, TAG_H4, TAG_H5, TAG_H6, TAG_LAST)) {
      // No heading open; ignore the token entirely.
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    } else {
      generate_implied_end_tags(parser, TAG_LAST);
      const Node* current_node = get_current_node(parser);
      bool success = node_tag_is(current_node, token->v.end_tag);
      if (!success) {
        // There're children of the heading currently open; close them below and
        // record a parse error.
        // TODO(jdtang): Add a way to distinguish this error case from the one
        // above.
        add_parse_error(parser, token);
      }
      do {
        current_node = pop_current_node(parser);
      } while (!node_tag_in(current_node, TAG_H1, TAG_H2, TAG_H3, TAG_H4, TAG_H5, TAG_H6, TAG_LAST));
      return success;
    }
  } else if (tag_is(token, kStartTag, TAG_A)) {
    bool success = true;
    int last_a;
    int has_matching_a = find_last_anchor_index(parser, &last_a);
    if (has_matching_a) {
      assert(has_matching_a == 1);
      add_parse_error(parser, token);
      adoption_agency_algorithm(parser, token, TAG_A);
      // The adoption agency algorithm usually removes all instances of <a>
      // from the list of active formatting elements, but in case it doesn't,
      // we're supposed to do this.  (The conditions where it might not are
      // listed in the spec.)
      if (find_last_anchor_index(parser, &last_a)) {
        void* last_element = vector_remove_at(parser, last_a, &state->_active_formatting_elements);
        vector_remove(parser, last_element, &state->_open_elements);
      }
      success = false;
    }
    reconstruct_active_formatting_elements(parser);
    add_formatting_element(parser, insert_element_from_token(parser, token));
    return success;
  } else if (tag_in(token, kStartTag, TAG_B, TAG_BIG, TAG_CODE, TAG_EM, TAG_FONT, TAG_I, TAG_S, TAG_SMALL, TAG_STRIKE, TAG_STRONG, TAG_TT, TAG_U, TAG_LAST)) {
    reconstruct_active_formatting_elements(parser);
    add_formatting_element(parser, insert_element_from_token(parser, token));
    return true;
  } else if (tag_is(token, kStartTag, TAG_NOBR)) {
    bool result = true;
    reconstruct_active_formatting_elements(parser);
    if (has_an_element_in_scope(parser, TAG_NOBR)) {
      result = false;
      add_parse_error(parser, token);
      adoption_agency_algorithm(parser, token, TAG_NOBR);
      reconstruct_active_formatting_elements(parser);
    }
    insert_element_from_token(parser, token);
    add_formatting_element(parser, get_current_node(parser));
    return result;
  } else if (tag_in(token, kEndTag, TAG_A, TAG_B, TAG_BIG, TAG_CODE, TAG_EM, TAG_FONT, TAG_I, TAG_NOBR, TAG_S, TAG_SMALL, TAG_STRIKE, TAG_STRONG, TAG_TT, TAG_U, TAG_LAST)) {
    return adoption_agency_algorithm(parser, token, token->v.end_tag);
  } else if (tag_in(token, kStartTag, TAG_APPLET, TAG_MARQUEE, TAG_OBJECT, TAG_LAST)) {
    reconstruct_active_formatting_elements(parser);
    insert_element_from_token(parser, token);
    add_formatting_element(parser, &kActiveFormattingScopeMarker);
    set_frameset_not_ok(parser);
    return true;
  } else if (tag_in(token, kEndTag, TAG_APPLET, TAG_MARQUEE, TAG_OBJECT, TAG_LAST)) {
    HTMLTag token_tag = token->v.end_tag;
    if (!has_an_element_in_table_scope(parser, token_tag)) {
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    implicitly_close_tags(parser, token, token_tag);
    clear_active_formatting_elements(parser);
    return true;
  } else if (tag_is(token, kStartTag, TAG_TABLE)) {
    if (get_document_node(parser)->v.document.doc_type_quirks_mode != DOCTYPE_QUIRKS) {
      maybe_implicitly_close_p_tag(parser, token);
    }
    insert_element_from_token(parser, token);
    set_frameset_not_ok(parser);
    set_insertion_mode(parser, INSERTION_MODE_IN_TABLE);
    return true;
  } else if (tag_in(token, kStartTag, TAG_AREA, TAG_BR, TAG_EMBED, TAG_IMG, TAG_IMAGE, TAG_KEYGEN, TAG_WBR, TAG_LAST)) {
    bool success = true;
    if (tag_is(token, kStartTag, TAG_IMAGE)) {
      success = false;
      add_parse_error(parser, token);
      token->v.start_tag.tag = TAG_IMG;
    }
    reconstruct_active_formatting_elements(parser);
    Node* node = insert_element_from_token(parser, token);
    if (tag_is(token, kStartTag, TAG_IMAGE)) {
      success = false;
      add_parse_error(parser, token);
      node->v.element.tag = TAG_IMG;
      node->parse_flags = INSERTION_FROM_IMAGE;
    }
    pop_current_node(parser);
    acknowledge_self_closing_tag(parser);
    set_frameset_not_ok(parser);
    return success;
  } else if (tag_is(token, kStartTag, TAG_INPUT)) {
    if (!attribute_matches(&token->v.start_tag.attributes, "type", "hidden")) {
      // Must be before the element is inserted, as that takes ownership of the
      // token's attribute vector.
      set_frameset_not_ok(parser);
    }
    reconstruct_active_formatting_elements(parser);
    insert_element_from_token(parser, token);
    pop_current_node(parser);
    acknowledge_self_closing_tag(parser);
    return true;
  } else if (tag_in(token, kStartTag, TAG_PARAM, TAG_SOURCE, TAG_TRACK, TAG_LAST)) {
    insert_element_from_token(parser, token);
    pop_current_node(parser);
    acknowledge_self_closing_tag(parser);
    return true;
  } else if (tag_is(token, kStartTag, TAG_HR)) {
    bool result = maybe_implicitly_close_p_tag(parser, token);
    insert_element_from_token(parser, token);
    pop_current_node(parser);
    acknowledge_self_closing_tag(parser);
    set_frameset_not_ok(parser);
    return result;
  } else if (tag_is(token, kStartTag, TAG_ISINDEX)) {
    add_parse_error(parser, token);
    if (parser->_parser_state->_form_element != NULL) {
      ignore_token(parser);
      return false;
    }
    acknowledge_self_closing_tag(parser);
    maybe_implicitly_close_p_tag(parser, token);
    set_frameset_not_ok(parser);

    Vector* token_attrs = &token->v.start_tag.attributes;
    Attribute* prompt_attr = get_attribute(token_attrs, "prompt");
    Attribute* action_attr = get_attribute(token_attrs, "action");
    Attribute* name_attr = get_attribute(token_attrs, "isindex");

    Node* form = insert_element_of_tag_type(parser, TAG_FORM, INSERTION_FROM_ISINDEX);
    if (action_attr) {
      vector_add(parser, action_attr, &form->v.element.attributes);
    }
    insert_element_of_tag_type(parser, TAG_HR, INSERTION_FROM_ISINDEX);
    pop_current_node(parser);   // <hr>

    insert_element_of_tag_type(parser, TAG_LABEL, INSERTION_FROM_ISINDEX);
    TextNodeBufferState* text_state = &parser->_parser_state->_text_node;
    text_state->_start_original_text = token->original_text.data;
    text_state->_start_position = token->position;
    text_state->_type = NODE_TEXT;
    if (prompt_attr) {
      int prompt_attr_length = strlen(prompt_attr->value);
      string_buffer_destroy(parser, &text_state->_buffer);
      text_state->_buffer.data = copy_stringz(parser, prompt_attr->value);
      text_state->_buffer.length = prompt_attr_length;
      text_state->_buffer.capacity = prompt_attr_length + 1;
      destroy_attribute(parser, prompt_attr);
    } else {
      StringPiece prompt_text = STRING("This is a searchable index. Enter search keywords: ");
      string_buffer_append_string(parser, &prompt_text, &text_state->_buffer);
    }

    Node* input = insert_element_of_tag_type(parser, TAG_INPUT, INSERTION_FROM_ISINDEX);
    for (int i = 0; i < token_attrs->length; ++i) {
      Attribute* attr = (Attribute*)token_attrs->data[i];
      if (attr != prompt_attr && attr != action_attr && attr != name_attr) {
        vector_add(parser, attr, &input->v.element.attributes);
      }
      token_attrs->data[i] = NULL;
    }

    // All attributes have been successfully transferred and nulled out at this
    // point, so the call to ignore_token will free the memory for it without
    // touching the attributes.
    ignore_token(parser);

    Attribute* name = (Attribute*)parser_allocate(parser, sizeof(Attribute));
    StringPiece name_str = STRING("name");
    StringPiece isindex_str = STRING("isindex");
    name->attr_namespace = ATTR_NAMESPACE_NONE;
    name->name = copy_stringz(parser, "name");
    name->value = copy_stringz(parser, "isindex");
    name->original_name = name_str;
    name->original_value = isindex_str;
    name->name_start = kEmptySrcPosition;
    name->name_end = kEmptySrcPosition;
    name->value_start = kEmptySrcPosition;
    name->value_end = kEmptySrcPosition;
    vector_add(parser, name, &input->v.element.attributes);

    pop_current_node(parser);   // <input>
    pop_current_node(parser);   // <label>
    insert_element_of_tag_type(parser, TAG_HR, INSERTION_FROM_ISINDEX);
    pop_current_node(parser);   // <hr>
    pop_current_node(parser);   // <form>
    return false;
  } else if (tag_is(token, kStartTag, TAG_TEXTAREA)) {
    run_generic_parsing_algorithm(parser, token, RCDATA);
    parser->_parser_state->_ignore_next_linefeed = true;
    set_frameset_not_ok(parser);
    return true;
  } else if (tag_is(token, kStartTag, TAG_XMP)) {
    bool result = maybe_implicitly_close_p_tag(parser, token);
    reconstruct_active_formatting_elements(parser);
    set_frameset_not_ok(parser);
    run_generic_parsing_algorithm(parser, token, RAWTEXT);
    return result;
  } else if (tag_is(token, kStartTag, TAG_IFRAME)) {
    set_frameset_not_ok(parser);
    run_generic_parsing_algorithm(parser, token, RAWTEXT);
    return true;
  } else if (tag_is(token, kStartTag, TAG_NOEMBED)) {
    run_generic_parsing_algorithm(parser, token, RAWTEXT);
    return true;
  } else if (tag_is(token, kStartTag, TAG_SELECT)) {
    reconstruct_active_formatting_elements(parser);
    insert_element_from_token(parser, token);
    set_frameset_not_ok(parser);
    InsertionMode state = parser->_parser_state->_insertion_mode;
    if (state == INSERTION_MODE_IN_TABLE || state == INSERTION_MODE_IN_CAPTION || state == INSERTION_MODE_IN_TABLE_BODY || state == INSERTION_MODE_IN_ROW || state == INSERTION_MODE_IN_CELL) {
      set_insertion_mode(parser, INSERTION_MODE_IN_SELECT_IN_TABLE);
    } else {
      set_insertion_mode(parser, INSERTION_MODE_IN_SELECT);
    }
    return true;
  } else if (tag_in(token, kStartTag, TAG_OPTION, TAG_OPTGROUP, TAG_LAST)) {
    if (node_tag_is(get_current_node(parser), TAG_OPTION)) {
      pop_current_node(parser);
    }
    reconstruct_active_formatting_elements(parser);
    insert_element_from_token(parser, token);
    return true;
  } else if (tag_in(token, kStartTag, TAG_RP, TAG_RT, TAG_LAST)) {
    bool success = true;
    if (has_an_element_in_scope(parser, TAG_RUBY)) {
      generate_implied_end_tags(parser, TAG_LAST);
    }
    if (!node_tag_is(get_current_node(parser), TAG_RUBY)) {
      add_parse_error(parser, token);
      success = false;
    }
    insert_element_from_token(parser, token);
    return success;
  } else if (tag_is(token, kEndTag, TAG_BR)) {
    add_parse_error(parser, token);
    reconstruct_active_formatting_elements(parser);
    insert_element_of_tag_type(parser, TAG_BR, INSERTION_CONVERTED_FROM_END_TAG);
    pop_current_node(parser);
    return false;
  } else if (tag_is(token, kStartTag, TAG_MATH)) {
    reconstruct_active_formatting_elements(parser);
    adjust_mathml_attributes(parser, token);
    adjust_foreign_attributes(parser, token);
    insert_foreign_element(parser, token, NAMESPACE_MATHML);
    if (token->v.start_tag.is_self_closing) {
      pop_current_node(parser);
      acknowledge_self_closing_tag(parser);
    }
    return true;
  } else if (tag_is(token, kStartTag, TAG_SVG)) {
    reconstruct_active_formatting_elements(parser);
    adjust_svg_attributes(parser, token);
    adjust_foreign_attributes(parser, token);
    insert_foreign_element(parser, token, NAMESPACE_SVG);
    if (token->v.start_tag.is_self_closing) {
      pop_current_node(parser);
      acknowledge_self_closing_tag(parser);
    }
    return true;
  } else if (tag_in(token, kStartTag, TAG_CAPTION, TAG_COL, TAG_COLGROUP, TAG_FRAME, TAG_HEAD, TAG_TBODY, TAG_TD, TAG_TFOOT, TAG_TH, TAG_THEAD, TAG_TR, TAG_LAST)) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (token->type == TOKEN_START_TAG) {
    reconstruct_active_formatting_elements(parser);
    insert_element_from_token(parser, token);
    return true;
  } else {
    assert(token->type == TOKEN_END_TAG);
    HTMLTag end_tag = token->v.end_tag;
    assert(state->_open_elements.length > 0);
    assert(node_tag_is((const Node*)state->_open_elements.data[0], TAG_HTML));
    // Walk up the stack of open elements until we find one that either:
    // a) Matches the tag name we saw
    // b) Is in the "special" category.
    // If we see a), implicitly close everything up to and including it.  If we
    // see b), then record a parse error, don't close anything (except the
    // implied end tags) and ignore the end tag token.
    for (int i = state->_open_elements.length; --i >= 0; ) {
      const Node* node = (const Node*)state->_open_elements.data[i];
      if (node->v.element.tag_namespace == NAMESPACE_HTML && node_tag_is(node, end_tag)) {
        generate_implied_end_tags(parser, end_tag);
        // TODO(jdtang): Do I need to add a parse error here?  The condition in
        // the spec seems like it's the inverse of the loop condition above, and
        // so would never fire.
        while (node != pop_current_node(parser));  // Pop everything.
        return true;
      } else if (is_special_node(node)) {
        add_parse_error(parser, token);
        ignore_token(parser);
        return false;
      }
    }
    // <html> is in the special category, so we should never get here.
    assert(0);
    return false;
  }
}

static bool handle_text(Parser* parser, Token* token) {
  if (token->type == TOKEN_CHARACTER || token->type == TOKEN_WHITESPACE) {
    insert_text_token(parser, token);
  } else {
    // We provide only bare-bones script handling that doesn't involve any of
    // the parser-pause/already-started/script-nesting flags or re-entrant
    // invocations of the tokenizer.  Because the intended usage of this library
    // is mostly for templating, refactoring, and static-analysis libraries, we
    // provide the script body as a text-node child of the <script> element.
    // This behavior doesn't support document.write of partial HTML elements,
    // but should be adequate for almost all other scripting support.
    if (token->type == TOKEN_EOF) {
      add_parse_error(parser, token);
      parser->_parser_state->_reprocess_current_token = true;
    }
    pop_current_node(parser);
    set_insertion_mode(parser, parser->_parser_state->_original_insertion_mode);
  }
  return true;
}

static bool handle_in_table(Parser* parser, Token* token) {
  ParserState* state = parser->_parser_state;
  if (token->type == TOKEN_CHARACTER || token->type == TOKEN_WHITESPACE) {
    // The "pending table character tokens" list described in the spec is
    // nothing more than the TextNodeBufferState.  We accumulate text tokens as
    // normal, except that when we go to flush them in the handle_in_table_text,
    // we set _foster_parent_insertions if there're non-whitespace characters in
    // the buffer.
    assert(state->_text_node._buffer.length == 0);
    state->_original_insertion_mode = state->_insertion_mode;
    state->_reprocess_current_token = true;
    set_insertion_mode(parser, INSERTION_MODE_IN_TABLE_TEXT);
    return true;
  } else if (token->type == TOKEN_DOCTYPE) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (token->type == TOKEN_COMMENT) {
    append_comment_node(parser, get_current_node(parser), token);
    return true;
  } else if (tag_is(token, kStartTag, TAG_CAPTION)) {
    clear_stack_to_table_context(parser);
    add_formatting_element(parser, &kActiveFormattingScopeMarker);
    insert_element_from_token(parser, token);
    set_insertion_mode(parser, INSERTION_MODE_IN_CAPTION);
    return true;
  } else if (tag_is(token, kStartTag, TAG_COLGROUP)) {
    clear_stack_to_table_context(parser);
    insert_element_from_token(parser, token);
    set_insertion_mode(parser, INSERTION_MODE_IN_COLUMN_GROUP);
    return true;
  } else if (tag_is(token, kStartTag, TAG_COL)) {
    clear_stack_to_table_context(parser);
    insert_element_of_tag_type(parser, TAG_COLGROUP, INSERTION_IMPLIED);
    parser->_parser_state->_reprocess_current_token = true;
    set_insertion_mode(parser, INSERTION_MODE_IN_COLUMN_GROUP);
    return true;
  } else if (tag_in(token, kStartTag, TAG_TBODY, TAG_TFOOT, TAG_THEAD, TAG_TD, TAG_TH, TAG_TR, TAG_LAST)) {
    clear_stack_to_table_context(parser);
    set_insertion_mode(parser, INSERTION_MODE_IN_TABLE_BODY);
    if (tag_in(token, kStartTag, TAG_TD, TAG_TH, TAG_TR, TAG_LAST)) {
      insert_element_of_tag_type(parser, TAG_TBODY, INSERTION_IMPLIED);
      state->_reprocess_current_token = true;
    } else {
      insert_element_from_token(parser, token);
    }
    return true;
  } else if (tag_is(token, kStartTag, TAG_TABLE)) {
    add_parse_error(parser, token);
    if (close_table(parser)) {
      parser->_parser_state->_reprocess_current_token = true;
    } else {
      ignore_token(parser);
    }
    return false;
  } else if (tag_is(token, kEndTag, TAG_TABLE)) {
    if (!close_table(parser)) {
      add_parse_error(parser, token);
      return false;
    }
    return true;
  } else if (tag_in(token, kEndTag, TAG_BODY, TAG_CAPTION, TAG_COL, TAG_COLGROUP, TAG_HTML, TAG_TBODY, TAG_TD, TAG_TFOOT, TAG_TH, TAG_THEAD, TAG_TR, TAG_LAST)) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (tag_in(token, kStartTag, TAG_STYLE, TAG_SCRIPT, TAG_LAST)) {
    return handle_in_head(parser, token);
  } else if (tag_is(token, kStartTag, TAG_INPUT) && attribute_matches(&token->v.start_tag.attributes, "type", "hidden")) {
    add_parse_error(parser, token);
    insert_element_from_token(parser, token);
    pop_current_node(parser);
    return false;
  } else if (tag_is(token, kStartTag, TAG_FORM)) {
    add_parse_error(parser, token);
    if (state->_form_element) {
      ignore_token(parser);
      return false;
    }
    state->_form_element = insert_element_from_token(parser, token);
    pop_current_node(parser);
    return false;
  } else if (token->type == TOKEN_EOF) {
    if (!node_tag_is(get_current_node(parser), TAG_HTML)) {
      add_parse_error(parser, token);
      return false;
    }
    return true;
  } else {
    add_parse_error(parser, token);
    state->_foster_parent_insertions = true;
    bool result = handle_in_body(parser, token);
    state->_foster_parent_insertions = false;
    return result;
  }
}

static bool handle_in_table_text(Parser* parser, Token* token) {
  if (token->type == TOKEN_NULL) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (token->type == TOKEN_CHARACTER || token->type == TOKEN_WHITESPACE) {
    insert_text_token(parser, token);
    return true;
  } else {
    ParserState* state = parser->_parser_state;
    StringBuffer* buffer = &state->_text_node._buffer;
    // Can't use strspn for this because GumboStringBuffers are not
    // null-terminated.
    // Note that TextNodeBuffer may contain UTF-8 characters, but the presence
    // of any one byte that is not whitespace means we flip the flag, so this
    // loop is still valid.
    for (int i = 0; i < buffer->length; ++i) {
      if (!isspace(buffer->data[i]) || buffer->data[i] == '\v') {
        state->_foster_parent_insertions = true;
        reconstruct_active_formatting_elements(parser);
        break;
      }
    }
    maybe_flush_text_node_buffer(parser);
    state->_foster_parent_insertions = false;
    state->_reprocess_current_token = true;
    state->_insertion_mode = state->_original_insertion_mode;
    return true;
  }
}

static bool handle_in_caption(Parser* parser, Token* token) {
  if (tag_in(token, kStartTag, TAG_CAPTION, TAG_COL, TAG_COLGROUP, TAG_TBODY, TAG_TD, TAG_TFOOT, TAG_TH, TAG_THEAD, TAG_TR, TAG_LAST) || tag_in(token, kEndTag, TAG_CAPTION, TAG_TABLE, TAG_LAST)) {
    if (!has_an_element_in_table_scope(parser, TAG_CAPTION)) {
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    if (!tag_is(token, kEndTag, TAG_CAPTION)) {
      add_parse_error(parser, token);
      parser->_parser_state->_reprocess_current_token = true;
    }
    generate_implied_end_tags(parser, TAG_LAST);
    bool result = true;
    if (!node_tag_is(get_current_node(parser), TAG_CAPTION)) {
      add_parse_error(parser, token);
      while (!node_tag_is(get_current_node(parser), TAG_CAPTION)) {
        pop_current_node(parser);
      }
      result = false;
    }
    pop_current_node(parser);  // The <caption> itself.
    clear_active_formatting_elements(parser);
    set_insertion_mode(parser, INSERTION_MODE_IN_TABLE);
    return result;
  } else if (tag_in(token, kEndTag, TAG_BODY, TAG_COL, TAG_COLGROUP, TAG_HTML, TAG_TBODY, TAG_TD, TAG_TFOOT, TAG_TH, TAG_THEAD, TAG_TR, TAG_LAST)) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else {
    return handle_in_body(parser, token);
  }
}

static bool handle_in_column_group(Parser* parser, Token* token) {
  if (token->type == TOKEN_WHITESPACE) {
    insert_text_token(parser, token);
    return true;
  } else if (token->type == TOKEN_DOCTYPE) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (token->type == TOKEN_COMMENT) {
    append_comment_node(parser, get_current_node(parser), token);
    return true;
  } else if (tag_is(token, kStartTag, TAG_HTML)) {
    return handle_in_body(parser, token);
  } else if (tag_is(token, kStartTag, TAG_COL)) {
    insert_element_from_token(parser, token);
    pop_current_node(parser);
    acknowledge_self_closing_tag(parser);
    return true;
  } else if (tag_is(token, kEndTag, TAG_COL)) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (token->type == TOKEN_EOF && get_current_node(parser) == parser->_output->root) {
    return true;
  } else {
    if (get_current_node(parser) == parser->_output->root) {
      add_parse_error(parser, token);
      return false;
    }
    assert(node_tag_is(get_current_node(parser), TAG_COLGROUP));
    pop_current_node(parser);
    set_insertion_mode(parser, INSERTION_MODE_IN_TABLE);
    if (!tag_is(token, kEndTag, TAG_COLGROUP)) {
      parser->_parser_state->_reprocess_current_token = true;
    }
    return true;
  }
}

static bool handle_in_table_body(Parser* parser, Token* token) {
  if (tag_is(token, kStartTag, TAG_TR)) {
    clear_stack_to_table_body_context(parser);
    insert_element_from_token(parser, token);
    set_insertion_mode(parser, INSERTION_MODE_IN_ROW);
    return true;
  } else if (tag_in(token, kStartTag, TAG_TD, TAG_TH, TAG_LAST)) {
    add_parse_error(parser, token);
    clear_stack_to_table_body_context(parser);
    insert_element_of_tag_type(parser, TAG_TR, INSERTION_IMPLIED);
    parser->_parser_state->_reprocess_current_token = true;
    set_insertion_mode(parser, INSERTION_MODE_IN_ROW);
    return false;
  } else if (tag_in(token, kEndTag, TAG_TBODY, TAG_TFOOT, TAG_THEAD, TAG_LAST)) {
    if (!has_an_element_in_table_scope(parser, token->v.end_tag)) {
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    clear_stack_to_table_body_context(parser);
    pop_current_node(parser);
    set_insertion_mode(parser, INSERTION_MODE_IN_TABLE);
    return true;
  } else if (tag_in(token, kStartTag, TAG_CAPTION, TAG_COL, TAG_COLGROUP, TAG_TBODY, TAG_TFOOT, TAG_THEAD, TAG_LAST) || tag_is(token, kEndTag, TAG_TABLE)) {
    if (!(has_an_element_in_table_scope(parser, TAG_TBODY) || has_an_element_in_table_scope(parser, TAG_THEAD) || has_an_element_in_table_scope(parser, TAG_TFOOT))) {
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    clear_stack_to_table_body_context(parser);
    pop_current_node(parser);
    set_insertion_mode(parser, INSERTION_MODE_IN_TABLE);
    parser->_parser_state->_reprocess_current_token = true;
    return true;
  } else if (tag_in(token, kEndTag, TAG_BODY, TAG_CAPTION, TAG_COL, TAG_TR, TAG_COLGROUP, TAG_HTML, TAG_TD, TAG_TH, TAG_LAST)){
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else {
    return handle_in_table(parser, token);
  }
}

static bool handle_in_row(Parser* parser, Token* token) {
  if (tag_in(token, kStartTag, TAG_TH, TAG_TD, TAG_LAST)) {
    clear_stack_to_table_row_context(parser);
    insert_element_from_token(parser, token);
    set_insertion_mode(parser, INSERTION_MODE_IN_CELL);
    add_formatting_element(parser, &kActiveFormattingScopeMarker);
    return true;
  } else if (tag_in(token, kStartTag, TAG_CAPTION, TAG_COLGROUP, TAG_TBODY, TAG_TFOOT, TAG_THEAD, TAG_TR, TAG_LAST) || tag_in(token, kEndTag, TAG_TR, TAG_TABLE, TAG_TBODY, TAG_TFOOT, TAG_THEAD, TAG_LAST)) {
    // This case covers 4 clauses of the spec, each of which say "Otherwise, act
    // as if an end tag with the tag name "tr" had been seen."  The differences
    // are in error handling and whether the current token is reprocessed.
    HTMLTag desired_tag = tag_in(token, kEndTag, TAG_TBODY, TAG_TFOOT, TAG_THEAD, TAG_LAST) ? token->v.end_tag : TAG_TR;
    if (!has_an_element_in_table_scope(parser, desired_tag)) {
      debug("Bailing because there is no tag %s in table scope.\nOpen elements:", normalized_tagname(desired_tag));
      for (int i = 0; i < parser->_parser_state->_open_elements.length; ++i) {
        const Node* node = (const Node*)parser->_parser_state->_open_elements.data[i];
        debug("%s\n", normalized_tagname(node->v.element.tag));
      }
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    clear_stack_to_table_row_context(parser);
    Node* last_element = pop_current_node(parser);
    assert(node_tag_is(last_element, TAG_TR));
    AVOID_UNUSED_VARIABLE_WARNING(last_element);
    set_insertion_mode(parser, INSERTION_MODE_IN_TABLE_BODY);
    if (!tag_is(token, kEndTag, TAG_TR)) {
      parser->_parser_state->_reprocess_current_token = true;
    }
    return true;
  } else if (tag_in(token, kEndTag, TAG_BODY, TAG_CAPTION, TAG_COL, TAG_COLGROUP, TAG_HTML, TAG_TD, TAG_TH, TAG_LAST)) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else {
    return handle_in_table(parser, token);
  }
}

static bool handle_in_cell(Parser* parser, Token* token) {
  if (tag_in(token, kEndTag, TAG_TD, TAG_TH, TAG_LAST)) {
    HTMLTag token_tag = token->v.end_tag;
    if (!has_an_element_in_table_scope(parser, token_tag)) {
      add_parse_error(parser, token);
      return false;
    }
    return close_table_cell(parser, token, token_tag);
  } else if (tag_in(token, kStartTag, TAG_CAPTION, TAG_COL, TAG_COLGROUP, TAG_TBODY, TAG_TD, TAG_TFOOT, TAG_TH, TAG_THEAD, TAG_TR, TAG_LAST)) {
    debug("Handling <td> in cell.\n");
    if (!has_an_element_in_table_scope(parser, TAG_TH) && !has_an_element_in_table_scope(parser, TAG_TD)) {
      debug("Bailing out because there's no <td> or <th> in scope.\n");
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    parser->_parser_state->_reprocess_current_token = true;
    return close_current_cell(parser, token);
  } else if (tag_in(token, kEndTag, TAG_BODY, TAG_CAPTION, TAG_COL, TAG_COLGROUP, TAG_HTML, TAG_LAST)) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (tag_in(token, kEndTag, TAG_TABLE, TAG_TBODY, TAG_TFOOT, TAG_THEAD, TAG_TR, TAG_LAST)) {
    if (!has_an_element_in_table_scope(parser, token->v.end_tag)) {
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    parser->_parser_state->_reprocess_current_token = true;
    return close_current_cell(parser, token);
  } else {
    return handle_in_body(parser, token);
  }
}

static bool handle_in_select(Parser* parser, Token* token) {
  if (token->type == TOKEN_NULL) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (token->type == TOKEN_CHARACTER || token->type == TOKEN_WHITESPACE) {
    insert_text_token(parser, token);
    return true;
  } else if (token->type == TOKEN_DOCTYPE) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (token->type == TOKEN_COMMENT) {
    append_comment_node(parser, get_current_node(parser), token);
    return true;
  } else if (tag_is(token, kStartTag, TAG_HTML)) {
    return handle_in_body(parser, token);
  } else if (tag_is(token, kStartTag, TAG_OPTION)) {
    if (node_tag_is(get_current_node(parser), TAG_OPTION)) {
      pop_current_node(parser);
    }
    insert_element_from_token(parser, token);
    return true;
  } else if (tag_is(token, kStartTag, TAG_OPTGROUP)) {
    if (node_tag_is(get_current_node(parser), TAG_OPTION)) {
      pop_current_node(parser);
    }
    if (node_tag_is(get_current_node(parser), TAG_OPTGROUP)) {
      pop_current_node(parser);
    }
    insert_element_from_token(parser, token);
    return true;
  } else if (tag_is(token, kEndTag, TAG_OPTGROUP)) {
    Vector* open_elements = &parser->_parser_state->_open_elements;
    if (node_tag_is((const Node*)get_current_node(parser), TAG_OPTION) && node_tag_is((const Node*)open_elements->data[open_elements->length - 2], TAG_OPTGROUP)) {
      pop_current_node(parser);
    }
    if (node_tag_is(get_current_node(parser), TAG_OPTGROUP)) {
      pop_current_node(parser);
      return true;
    } else {
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
  } else if (tag_is(token, kEndTag, TAG_OPTION)) {
    if (node_tag_is(get_current_node(parser), TAG_OPTION)) {
      pop_current_node(parser);
      return true;
    } else {
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
  } else if (tag_is(token, kEndTag, TAG_SELECT)) {
    if (!has_an_element_in_select_scope(parser, TAG_SELECT)) {
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    close_current_select(parser);
    return true;
  } else if (tag_is(token, kStartTag, TAG_SELECT)) {
    add_parse_error(parser, token);
    ignore_token(parser);
    close_current_select(parser);
    return false;
  } else if (tag_in(token, kStartTag, TAG_INPUT, TAG_KEYGEN, TAG_TEXTAREA, TAG_LAST)) {
    add_parse_error(parser, token);
    if (!has_an_element_in_select_scope(parser, TAG_SELECT)) {
      ignore_token(parser);
    } else {
      close_current_select(parser);
      parser->_parser_state->_reprocess_current_token = true;
    }
    return false;
  } else if (tag_is(token, kStartTag, TAG_SCRIPT)) {
    return handle_in_head(parser, token);
  } else if (token->type == TOKEN_EOF) {
    if (get_current_node(parser) != parser->_output->root) {
      add_parse_error(parser, token);
      return false;
    }
    return true;
  } else {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  }
}

static bool handle_in_select_in_table(Parser* parser, Token* token) {
  if (tag_in(token, kStartTag, TAG_CAPTION, TAG_TABLE, TAG_TBODY, TAG_TFOOT, TAG_THEAD, TAG_TR, TAG_TD, TAG_TH, TAG_LAST)) {
    add_parse_error(parser, token);
    close_current_select(parser);
    parser->_parser_state->_reprocess_current_token = true;
    return false;
  } else if (tag_in(token, kEndTag, TAG_CAPTION, TAG_TABLE, TAG_TBODY, TAG_TFOOT, TAG_THEAD, TAG_TR, TAG_TD, TAG_TH, TAG_LAST)) {
    add_parse_error(parser, token);
    if (has_an_element_in_table_scope(parser, token->v.end_tag)) {
      close_current_select(parser);
      reset_insertion_mode_appropriately(parser);
      parser->_parser_state->_reprocess_current_token = true;
    } else {
      ignore_token(parser);
    }
    return false;
  } else {
    return handle_in_select(parser, token);
  }
}

static bool handle_after_body(Parser* parser, Token* token) {
  if (token->type == TOKEN_WHITESPACE || tag_is(token, kStartTag, TAG_HTML)) {
    return handle_in_body(parser, token);
  } else if (token->type == TOKEN_COMMENT) {
    Node* html_node = parser->_output->root;
    assert(html_node != NULL);
    append_comment_node(parser, html_node, token);
    return true;
  } else if (token->type == TOKEN_DOCTYPE) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (tag_is(token, kEndTag, TAG_HTML)) {
    // TODO(jdtang): Handle fragment parsing algorithm case.
    set_insertion_mode(parser, INSERTION_MODE_AFTER_AFTER_BODY);
    Node* html = (Node*)parser->_parser_state->_open_elements.data[0];
    assert(node_tag_is(html, TAG_HTML));
    record_end_of_element(parser->_parser_state->_current_token, &html->v.element);
    return true;
  } else if (token->type == TOKEN_EOF) {
    return true;
  } else {
    add_parse_error(parser, token);
    set_insertion_mode(parser, INSERTION_MODE_IN_BODY);
    parser->_parser_state->_reprocess_current_token = true;
    return false;
  }
}

static bool handle_in_frameset(Parser* parser, Token* token) {
  if (token->type == TOKEN_WHITESPACE) {
    insert_text_token(parser, token);
    return true;
  } else if (token->type == TOKEN_COMMENT) {
    append_comment_node(parser, get_current_node(parser), token);
    return true;
  } else if (token->type == TOKEN_DOCTYPE) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (tag_is(token, kStartTag, TAG_HTML)) {
    return handle_in_body(parser, token);
  } else if (tag_is(token, kStartTag, TAG_FRAMESET)) {
    insert_element_from_token(parser, token);
    return true;
  } else if (tag_is(token, kEndTag, TAG_FRAMESET)) {
    if (node_tag_is(get_current_node(parser), TAG_HTML)) {
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    }
    pop_current_node(parser);
    // TODO(jdtang): Add a condition to ignore this for the fragment parsing
    // algorithm.
    if (!node_tag_is(get_current_node(parser), TAG_FRAMESET)) {
      set_insertion_mode(parser, INSERTION_MODE_AFTER_FRAMESET);
    }
    return true;
  } else if (tag_is(token, kStartTag, TAG_FRAME)) {
    insert_element_from_token(parser, token);
    pop_current_node(parser);
    acknowledge_self_closing_tag(parser);
    return true;
  } else if (tag_is(token, kStartTag, TAG_NOFRAMES)) {
    return handle_in_head(parser, token);
  } else if (token->type == TOKEN_EOF) {
    if (!node_tag_is(get_current_node(parser), TAG_HTML)) {
      add_parse_error(parser, token);
      return false;
    }
    return true;
  } else {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  }
}

static bool handle_after_frameset(Parser* parser, Token* token) {
  if (token->type == TOKEN_WHITESPACE) {
    insert_text_token(parser, token);
    return true;
  } else if (token->type == TOKEN_COMMENT) {
    append_comment_node(parser, get_current_node(parser), token);
    return true;
  } else if (token->type == TOKEN_DOCTYPE) {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  } else if (tag_is(token, kStartTag, TAG_HTML)) {
    return handle_in_body(parser, token);
  } else if (tag_is(token, kEndTag, TAG_HTML)) {
    set_insertion_mode(parser, INSERTION_MODE_AFTER_AFTER_FRAMESET);
    return true;
  } else if (tag_is(token, kStartTag, TAG_NOFRAMES)) {
    return handle_in_head(parser, token);
  } else if (token->type == TOKEN_EOF) {
    return true;
  } else {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  }
}

static bool handle_after_after_body(Parser* parser, Token* token) {
  if (token->type == TOKEN_COMMENT) {
    append_comment_node(parser, get_document_node(parser), token);
    return true;
  } else if (token->type == TOKEN_DOCTYPE || token->type == TOKEN_WHITESPACE || tag_is(token, kStartTag, TAG_HTML)) {
    return handle_in_body(parser, token);
  } else if (token->type == TOKEN_EOF) {
    return true;
  } else {
    add_parse_error(parser, token);
    set_insertion_mode(parser, INSERTION_MODE_IN_BODY);
    parser->_parser_state->_reprocess_current_token = true;
    return false;
  }
}

static bool handle_after_after_frameset(Parser* parser, Token* token) {
  if (token->type == TOKEN_COMMENT) {
    append_comment_node(parser, get_document_node(parser), token);
    return true;
  } else if (token->type == TOKEN_DOCTYPE || token->type == TOKEN_WHITESPACE || tag_is(token, kStartTag, TAG_HTML)) {
    return handle_in_body(parser, token);
  } else if (token->type == TOKEN_EOF) {
    return true;
  } else if (tag_is(token, kStartTag, TAG_NOFRAMES)) {
    return handle_in_head(parser, token);
  } else {
    add_parse_error(parser, token);
    ignore_token(parser);
    return false;
  }
}

// Function pointers for each insertion mode.  Keep in sync with insertion_mode.h.
typedef bool (*TokenHandler)(Parser* parser, Token* token);
static const TokenHandler kTokenHandlers[] = {
  handle_initial,
  handle_before_html,
  handle_before_head,
  handle_in_head,
  handle_in_head_noscript,
  handle_after_head,
  handle_in_body,
  handle_text,
  handle_in_table,
  handle_in_table_text,
  handle_in_caption,
  handle_in_column_group,
  handle_in_table_body,
  handle_in_row,
  handle_in_cell,
  handle_in_select,
  handle_in_select_in_table,
  handle_after_body,
  handle_in_frameset,
  handle_after_frameset,
  handle_after_after_body,
  handle_after_after_frameset
};

static bool handle_html_content(Parser* parser, Token* token) {
  return kTokenHandlers[parser->_parser_state->_insertion_mode](
      parser, token);
}

static bool handle_in_foreign_content(Parser* parser, Token* token) {
  switch (token->type) {
    case TOKEN_NULL:
      add_parse_error(parser, token);
      token->type = TOKEN_CHARACTER;
      token->v.character = kUtf8ReplacementChar;
      insert_text_token(parser, token);
      return false;
    case TOKEN_WHITESPACE:
      insert_text_token(parser, token);
      return true;
    case TOKEN_CHARACTER:
      insert_text_token(parser, token);
      set_frameset_not_ok(parser);
      return true;
    case TOKEN_COMMENT:
      append_comment_node(parser, get_current_node(parser), token);
      return true;
    case TOKEN_DOCTYPE:
      add_parse_error(parser, token);
      ignore_token(parser);
      return false;
    default:
      // Fall through to the if-statements below.
      break;
  }
  // Order matters for these clauses.
  if (tag_in(token, kStartTag, TAG_B, TAG_BIG, TAG_BLOCKQUOTE, TAG_BODY, TAG_BR, TAG_CENTER, TAG_CODE, TAG_DD, TAG_DIV, TAG_DL, TAG_DT, TAG_EM, TAG_EMBED, TAG_H1, TAG_H2, TAG_H3, TAG_H4, TAG_H5, TAG_H6, TAG_HEAD, TAG_HR, TAG_I, TAG_IMG, TAG_LI,
             TAG_LISTING, TAG_MENU, TAG_META, TAG_NOBR, TAG_OL, TAG_P, TAG_PRE, TAG_RUBY, TAG_S, TAG_SMALL, TAG_SPAN, TAG_STRONG, TAG_STRIKE, TAG_SUB, TAG_SUP, TAG_TABLE, TAG_TT, TAG_U, TAG_UL, TAG_VAR, TAG_LAST) ||
            (tag_is(token, kStartTag, TAG_FONT) && (token_has_attribute(token, "color") || token_has_attribute(token, "face") || token_has_attribute(token, "size")))) {
    add_parse_error(parser, token);
    do {
      pop_current_node(parser);
    } while(!(is_mathml_integration_point(get_current_node(parser)) || is_html_integration_point(get_current_node(parser)) || get_current_node(parser)->v.element.tag_namespace == NAMESPACE_HTML));
    parser->_parser_state->_reprocess_current_token = true;
    return false;
  } else if (token->type == TOKEN_START_TAG) {
    const NameSpaceEnum current_namespace = get_current_node(parser)->v.element.tag_namespace;
    if (current_namespace == NAMESPACE_MATHML) {
      adjust_mathml_attributes(parser, token);
    }
    if (current_namespace == NAMESPACE_SVG) {
      // Tag adjustment is left to the gumbo_normalize_svg_tagname helper function.
      adjust_svg_attributes(parser, token);
    }
    adjust_foreign_attributes(parser, token);
    insert_foreign_element(parser, token, current_namespace);
    if (token->v.start_tag.is_self_closing) {
      pop_current_node(parser);
      acknowledge_self_closing_tag(parser);
    }
    return true;
  // </script> tags are handled like any other end tag, putting the script's
  // text into a text node child and closing the current node.
  } else {
    assert(token->type == TOKEN_END_TAG);
    Node* node = get_current_node(parser);
    assert(node != NULL);
    StringPiece token_tagname = token->original_text;
    StringPiece node_tagname = node->v.element.original_tag;
    tag_from_original_text(&token_tagname);
    tag_from_original_text(&node_tagname);

    bool is_success = true;
    if (!string_equals_ignore_case(&node_tagname, &token_tagname)) {
      add_parse_error(parser, token);
      is_success = false;
    }
    int i = parser->_parser_state->_open_elements.length;
    for( --i; i > 0; ) {
      // Here we move up the stack until we find an HTML element (in which
      // case we do nothing) or we find the element that we're about to
      // close (in which case we pop everything we've seen until that point.)
      debug("Foreign %.*s node at %d.\n", node_tagname.length, node_tagname.data, i);
      if (string_equals_ignore_case(&node_tagname, &token_tagname)) {
        debug("Matches.\n");
        while (pop_current_node(parser) != node) {
          // Pop all the nodes below the current one.  Node is guaranteed to
          // be an element on the stack of open elements (set below), so
          // this loop is guaranteed to terminate.
        }
        return is_success;
      }
      --i;
      node = (Node*)parser->_parser_state->_open_elements.data[i];
      if (node->v.element.tag_namespace == NAMESPACE_HTML) {
        // Must break before gumbo_tag_from_original_text to avoid passing
        // parser-inserted nodes through.
        break;
      }
      node_tagname = node->v.element.original_tag;
      tag_from_original_text(&node_tagname);
    }
    assert(node->v.element.tag_namespace == NAMESPACE_HTML);
    // We can't call handle_token directly because the current node is still in
    // the SVG namespace, so it would re-enter this and result in infinite
    // recursion.
    return handle_html_content(parser, token) && is_success;
  }
}

static bool handle_token(Parser* parser, Token* token) {
  if (parser->_parser_state->_ignore_next_linefeed && token->type == TOKEN_WHITESPACE && token->v.character == '\n') {
    parser->_parser_state->_ignore_next_linefeed = false;
    ignore_token(parser);
    return true;
  }
  // This needs to be reset both here and in the conditional above to catch both
  // the case where the next token is not whitespace (so we don't ignore
  // whitespace in the middle of <pre> tags) and where there are multiple
  // whitespace tokens (so we don't ignore the second one).
  parser->_parser_state->_ignore_next_linefeed = false;

  if (tag_is(token, kEndTag, TAG_BODY)) {
    parser->_parser_state->_closed_body_tag = true;
  }
  if (tag_is(token, kEndTag, TAG_HTML)) {
    parser->_parser_state->_closed_html_tag = true;
  }

  const Node* current_node = get_current_node(parser);
  assert(!current_node || current_node->type == NODE_ELEMENT);
  if (current_node) {
    debug("Current node: <%s>.\n", normalized_tagname(current_node->v.element.tag));
  }
  if (!current_node || current_node->v.element.tag_namespace == NAMESPACE_HTML || (is_mathml_integration_point(current_node) &&(token->type == TOKEN_CHARACTER || token->type == TOKEN_WHITESPACE || token->type == TOKEN_NULL ||
     (token->type == TOKEN_START_TAG && !tag_in(token, kStartTag, TAG_MGLYPH, TAG_MALIGNMARK, TAG_LAST)))) || (current_node->v.element.tag_namespace == NAMESPACE_MATHML && node_tag_is(current_node, TAG_ANNOTATION_XML) &&
        tag_is(token, kStartTag, TAG_SVG)) || (is_html_integration_point(current_node) && (token->type == TOKEN_START_TAG || token->type == TOKEN_CHARACTER || token->type == TOKEN_NULL || token->type == TOKEN_WHITESPACE)) ||
      token->type == TOKEN_EOF) {
    return handle_html_content(parser, token);
  } else {
    return handle_in_foreign_content(parser, token);
  }
}

Output* parse(const char* buffer) {
  return parse_with_options(&kDefaultOptions, buffer, strlen(buffer));
}

Output* parse_with_options(const Options* options, const char* buffer, size_t length) {
  Parser parser;
  parser._options = options;
  output_init(&parser);
  tokenizer_state_init(&parser, buffer, length);
  parser_state_init(&parser);

  ParserState* state = parser._parser_state;
  debug("Parsing %.*s.\n", length, buffer);

  // Sanity check so that infinite loops die with an assertion failure instead
  // of hanging the process before we ever get an error.
  int loop_count = 0;

  Token token;
  bool has_error = false;
  do {
    if (state->_reprocess_current_token) {
      state->_reprocess_current_token = false;
    } else {
      Node* current_node = get_current_node(&parser);
      tokenizer_set_is_current_node_foreign(&parser, current_node && current_node->v.element.tag_namespace != NAMESPACE_HTML);
      has_error = !lex(&parser, &token) || has_error;
    }
    const char* token_type = "text";
    switch (token.type) {
      case TOKEN_DOCTYPE:
        token_type = "doctype";
        break;
      case TOKEN_START_TAG:
        token_type = normalized_tagname(token.v.start_tag.tag);
        break;
      case TOKEN_END_TAG:
        token_type = normalized_tagname(token.v.end_tag);
        break;
      case TOKEN_COMMENT:
        token_type = "comment";
        break;
      default:
        break;
    }
    debug("Handling %s token @%d:%d in state %d.\n", (char*) token_type, token.position.line, token.position.column, state->_insertion_mode);

    state->_current_token = &token;
    state->_self_closing_flag_acknowledged = !(token.type == TOKEN_START_TAG && token.v.start_tag.is_self_closing);

    has_error = !handle_token(&parser, &token) || has_error;

    // Check for memory leaks when ownership is transferred from start tag
    // tokens to nodes.
    assert(state->_reprocess_current_token || token.type != TOKEN_START_TAG || token.v.start_tag.attributes.data == NULL);

    if (!state->_self_closing_flag_acknowledged) {
      Error* error = add_parse_error(&parser, &token);
      if (error) {
        error->type = ERR_UNACKNOWLEDGED_SELF_CLOSING_TAG;
      }
    }

    ++loop_count;
    assert(loop_count < 1000000000);

  } while ((token.type != TOKEN_EOF || state->_reprocess_current_token) && !(options->stop_on_first_error && has_error));

  finish_parsing(&parser);
  // For API uniformity reasons, if the doctype still has nulls, convert them to
  // empty strings.
  Document* doc_type = &parser._output->document->v.document;
  if (doc_type->name == NULL) {
    doc_type->name = copy_stringz(&parser, "");
  }
  if (doc_type->public_identifier == NULL) {
    doc_type->public_identifier = copy_stringz(&parser, "");
  }
  if (doc_type->system_identifier == NULL) {
    doc_type->system_identifier = copy_stringz(&parser, "");
  }

  parser_state_destroy(&parser);
  tokenizer_state_destroy(&parser);
  return parser._output;
}

/*void destroy_node(Options* options, Node* node) {
  // Need a dummy GumboParser because the allocator comes along with the
  // options object.
  Parser parser;
  parser._options = options;
  destroy_node(&parser, node);
}*/

void destroy_output(const Options* options, Output* output) {
  // Need a dummy GumboParser because the allocator comes along with the
  // options object.
  Parser parser;
  parser._options = options;
  destroy_node(&parser, output->document);
  for (int i = 0; i < output->errors.length; ++i) {
    error_destroy(&parser, (Error*)output->errors.data[i]);
  }
  vector_destroy(&parser, &output->errors);
  parser_deallocate(&parser, output);
}
