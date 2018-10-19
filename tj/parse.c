#include <wchar.h>
#include "parse.h"
#include "token.h"
#include "mem.h"

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

json_node * parseRegularVA() {
    json_token * t = matchNext(TT_LBRACE | TT_LBRACKET | TT_STRING | TT_NUM | TT_TRUE | TT_FALSE | TT_NULL);

    json_node *node;
    json_node *res = new(sizeof(json_node));

    switch (t->type) {
        case TT_LBRACKET:
            node = parseRegularAA();
            matchNext(TT_RBRACKET);
            res->type = NT_NODE_ARRAY;
            res->array_len = node->array_len;
            res->node_array = node->node_array;
            break;

        case TT_LBRACE:
            node = parseRegularOA();
            matchNext(TT_RBRACE);
            res->type = NT_PAIR_ARRAY;
            res->array_len = node->array_len;
            res->pair_array = node->pair_array;
            break;

        case TT_STRING:
            res->type = NT_STRING;
            res->string = t->string;
            break;

        case TT_NUM:
            res->type = NT_NUM;
            res->num = t->num;
            break;

        case TT_TRUE:
            res->type = NT_TRUE;
            break;

        case TT_FALSE:
            res->type = NT_FALSE;
            break;

        case TT_NULL:
            res->type = NT_NULL;
            break;

        default:
            /* may not run into here */
            return NULL;
    }

    return res;
}

/*
 *
RG_AA := TT_LBRACE RG_OA TT_RBRACE RG_AA’
        | ε

RG_AA’ := TT_COMMA TT_LBRACE RG_OA TT_RBRACE RG_AA’
        | ε
 *
 */

json_node * parseRegularAA() {
    json_node *res = new(sizeof(json_node));
    res->array_len = 0;
    res->type = NT_NODE_ARRAY;
    res->node_array = new(sizeof(json_node) * MAX_POINTER_COUNT);

    json_node *node;
    json_token *t = matchNext(TT_ANY);

    if (t->type == TT_LBRACE) {
        node = parseRegularOA();
        matchNext(TT_RBRACE);
        res->node_array[res->array_len++] = node;
        parseRegularAA1(res);
        return res;

    } else {
        rollback();
        return NULL;
    }
}

json_node * parseRegularAA1(json_node *left) {
    json_node *node;
    json_token *t = matchNext(TT_ANY);
    if (t->type == TT_COMMA) {
        matchNext(TT_LBRACE);
        node = parseRegularOA();
        left->node_array[left->array_len++] = node;
        matchNext(TT_RBRACE);
        parseRegularAA1(left);
        return left;

    } else {
        rollback();
        return NULL;
    }

}

/*
 *

 RG_OA := RG_KVA RG_OA’
        | ε
 RG_OA’ := TT_COMMA RG_KVA RG_OA’
        | ε

 RG_KVA := TT_STRING TT_COLON RG_VA
        | ε

 *
 */

json_node * parseRegularOA() {
    json_node *res = new(sizeof(json_node));
    res->type = NT_PAIR_ARRAY;
    res->array_len = 0;
    res->pair_array = new(sizeof(json_pair) * MAX_POINTER_COUNT);


    json_pair *pair;
    if ((pair = parseRegularKVA()) != NULL) {
        res->pair_array[res->array_len++] = pair;
        parseRegularOA1(res);
        return res;
    } else {
        return NULL;
    }
}

json_node * parseRegularOA1(json_node *left) {
    json_token *t = matchNext(TT_ANY);
    json_pair *pair;
    if (t->type == TT_COMMA) {
        pair = parseRegularKVA();
        if (duplicateKey(left->pair_array, left->array_len, pair->key)) {
            printError(L"found duplicate key \"%ls\" before the end of key-value pair", pair->key);
        }
        left->pair_array[left->array_len++] = pair;
        parseRegularOA1(left);
        return left;

    } else {
        rollback();
        return NULL;
    }
}

json_pair * parseRegularKVA() {
    json_pair *res = new(sizeof(json_pair));
    json_node *val;
    json_token *t = matchNext(TT_ANY);
    if (t->type == TT_STRING) {
        matchNext(TT_COLON);
        val = parseRegularVA();
        res->key = t->string;
        res->val = val;
        return res;

    } else {
        rollback();
        return NULL;
    }
}

json_node * parse() {
    json_token *t = matchNext(TT_TRUE | TT_FALSE | TT_NUM | TT_STRING | TT_NULL | TT_LBRACE | TT_LBRACKET);
    json_node *res = NULL;
    switch (t->type) {
        case TT_TRUE:
        case TT_FALSE:
        case TT_STRING:
        case TT_NULL:
        case TT_NUM:
            rollback();
            res = parseRegularVA();
            break;

        case TT_LBRACE:
            res = parseRegularOA();
            matchNext(TT_RBRACE);
            break;

        case TT_LBRACKET:
            res = parseRegularAA();
            matchNext(TT_RBRACKET);
            break;

        default:
            terminate(1);
    }

    return res;
}

void setParser(wchar_t *src) {
    extern wchar_t *g_str_buff;
    g_str_buff = src;
    initTokenizer();
    initMem();
}

void destroyParser() {
    destroyTokenizer();
    destroyMem();
}

int duplicateKey(json_pair **pairs, int len, wchar_t *key) {
    int i;
    for (i = 0; i < len; i++) {
        if (wcscmp(pairs[i]->key, key) == 0) {
            return 1;
        }
    }
    return 0;
}
