#ifndef TOKEN_TYPE_H_
#define TOKEN_TYPE_H_

#ifdef __cplusplus
extern "C"{
#endif

typedef enum {
    TOKEN_DOCTYPE,
    TOKEN_START_TAG,
    TOKEN_END_TAG,
    TOKEN_COMMENT,
    TOKEN_WHITESPACE,
    TOKEN_CHARACTER,
    TOKEN_NULL,
    TOKEN_EOF
}GTokenType;

#ifdef __cplusplus
} //extern C
#endif

#endif // TOKEN_TYPE_H_
