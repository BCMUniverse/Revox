#ifndef PARSER_H_
#define PARSER_H_

#ifdef __cplusplus
extern "C"{
#endif

struct InternalParserState;
struct InternalOutput;
struct InternalOptions;
struct InternalTokenizerState;

typedef struct InternalParser{
    const struct InternalOptions* _options;
    struct InternalOutput* _output;
    struct InternalTokenizerState* _tokenizer_state;
    struct InternalParserState* _parser_state;
}Parser;

void destroy_output(const Options* options, Output* output);

#ifdef __cplusplus
}
#endif

#endif // PARSER_H_
