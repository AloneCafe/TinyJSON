/*
 * token.h: the data structure and function of JSON tokenizer
 *
 */

#ifndef TINYJSON_TOKEN_H
#define TINYJSON_TOKEN_H

#include <stdlib.h>
#include <stdio.h>

#define MAX_STRING_LENGTH 1000  /* max string length */

typedef enum _json_token_type{
    TERMINATOR = 0, /* parse reach the end of character flow */
    TT_LBRACE = 1, /* { */
    TT_RBRACE = 2, /* } */
    TT_LBRACKET = 4, /* [ */
    TT_RBRACKET = 8, /* ] */
    TT_COMMA = 16, /* , */
    TT_COLON = 32, /* : */
    TT_NUM = 64, /* number token */
    TT_STRING = 128,  /* string token (string) */
    TT_TRUE = 256,    /* true token */
    TT_FALSE = 512,   /* false token */
    TT_NULL = 1024,/* null token */
    TT_ANY = TT_LBRACE + TT_RBRACE + TT_LBRACKET + TT_RBRACKET + TT_COMMA + TT_COLON + TT_NUM + TT_STRING + TT_TRUE + TT_FALSE + TT_NULL
}json_token_type;

typedef struct _json_token {
    json_token_type type;
    size_t char_pos;    /* reserve field */
    union {
        long double num;
        wchar_t *string;
    };
}json_token;

void printError(const wchar_t *format, ...);   /* print error output to stderr, then exit program with a non-zero return value */
void initTokenizer();   /* initializing the tokenizer */
void destroyTokenizer();    /* free memory */
json_token * next();   /* get the next token */
void rollback();   /* get the last token */
wchar_t * getTokenTypeName(json_token_type type); /* get the name string of token type */
json_token * matchNext(json_token_type type); /* get the next token, match the except token, if not equal, exit program with a error message output */
void terminate(int exitcode);



#endif //TINYJSON_TOKEN_H
