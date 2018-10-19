/*
 * parse.h: the data structure and function of JSON parser
 *
 */

#ifndef TINYJSON_PARSE_H
#define TINYJSON_PARSE_H

#include <stddef.h>
#include "token.h"

/*
 *
    RG_VA := TT_LBRACKET RG_AA TT_RBRACKET
        | TT_LBRACE RG_OA TT_RBRACE
        | TT_STRING
        | TT_NUM
        | TT_TRUE
        | TT_FALSE
        | TT_NULL

    RG_AA := TT_LBRACE RG_OA TT_RBRACE RG_AA’
            | ε

    RG_AA’ := TT_COMMA TT_LBRACE RG_OA TT_RBRACE RG_AA’
            | ε

    RG_OA := RG_KVA RG_OA’
            | ε

    RG_OA’ := TT_COMMA RG_KVA RG_OA’
            | ε

    RG_KVA := TT_STRING TT_COLON RG_VA
            | ε
 *
 */

enum _json_node_type;
struct _json_node;
struct _json_pair;


typedef enum _json_node_type {

    /* value type */
    NT_NUM,
    NT_STRING,
    NT_TRUE,
    NT_FALSE,
    NT_NULL,

    /* combined type */
    NT_NODE_ARRAY,
    NT_PAIR_ARRAY

}json_node_type;

typedef struct _json_node {
    json_node_type type;
    int array_len;
    union {
        /* value type */
        long double num;
        wchar_t *string;

        /* combined type */
        struct _json_node **node_array; /* [ elem , elem ] */
        struct _json_pair **pair_array; /* { elem , elem } */
    };

}json_node;

typedef struct _json_pair {
    wchar_t *key;
    struct _json_node *val;
}json_pair;

void setParser(wchar_t *src);
void destroyParser();

json_node * parseRegularVA();
json_node * parseRegularAA();
json_node * parseRegularAA1(json_node *left);
json_node * parseRegularOA();
json_node * parseRegularOA1(json_node *left);
json_pair * parseRegularKVA();
json_node * parse();

int duplicateKey(json_pair **pairs, int len, wchar_t *key);

#endif //TINYJSON_PARSE_H
