#include "token.h"
#include "mem.h"
#include "parse.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>

int cl; /* current line (begin with 0) */
int cc;  /* current char position at current line (begin with 0) */
int ccp; /* current char position at character flow (begin with 0) */
int ctp; /* current token position (begin with 0) */
json_token *token_base;  /* base pointer of token flow */
int tc;  /* token count */
FILE *fp;   /* FILE structure pointer */

wchar_t *g_str_buff;


void printError(const wchar_t *format, ...) {
    va_list vl;
    va_start(vl, format);
    fwprintf(stderr, L"parse error at line %d, column %d: ", cl + 1, cc);
    vfwprintf(stderr, format, vl);
    fwprintf(stderr, L"\n");
    va_end(vl);
    terminate(1);
}

void initTokenizer() {
    if (token_base)
        free(token_base);
    token_base = malloc(sizeof(json_token) * 1000000);
    ctp = 0;
    ccp = 0;
    /* g_str_buff = ... */
}

void destroyTokenizer() {
    free(token_base);
}


json_token * next() {
    int state = 0;
    wchar_t *buff = new(sizeof(wchar_t) * MAX_STRING_LENGTH), *buff_p = buff;
    json_token t = { 0 };

    if (ctp < tc) {
        return &token_base[ctp++];
    }

    tc += 1;

    while(1) {
        cc += 1;
        switch (state) {
            case 0: /* initial state */
                if (g_str_buff[ccp] >= '0' && g_str_buff[ccp] <= '9') {   /* handle number */
                    *buff_p++ = g_str_buff[ccp];
                    ccp++;
                    state = 2;
                    break;
                }

                switch (g_str_buff[ccp++]) {   /* handle the first character */
                    case 't':
                        state = 4;
                        break; /* handle bool value, 't'rue */
                    case 'f':
                        state = 5;
                        break; /* handle bool value, 'f'alse */
                    case 'n':
                        state = 6;
                        break; /* handle 'n'ull */
                    case '{':
                        t.type = TT_LBRACE;
                        token_base[ctp] = t;
                        return &token_base[ctp++];
                    case '}':
                        t.type = TT_RBRACE;
                        token_base[ctp] = t;
                        return &token_base[ctp++];
                    case '[':
                        t.type = TT_LBRACKET;
                        token_base[ctp] = t;
                        return &token_base[ctp++];
                    case ']':
                        t.type = TT_RBRACKET;
                        token_base[ctp] = t;
                        return &token_base[ctp++];
                    case ',':
                        t.type = TT_COMMA;
                        token_base[ctp] = t;
                        return &token_base[ctp++];
                    case ':':
                        t.type = TT_COLON;
                        token_base[ctp] = t;
                        return &token_base[ctp++];
                    case '\"':
                        state = 1;
                        break;
                    case ' ':
                        break;
                    case '\n':
                        cl += 1;
                        cc = 0;
                        break;
                    case '\t':
                        break;
                    case '\0':
                        token_base[ctp] = t;
                        return &token_base[ctp++];
                    default:
                        printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 1]);
                }
                break;

            case 1: /* handle string */
                switch (g_str_buff[ccp++]) {
                    case '\"':
                        t.type = TT_STRING;
                        *buff_p = '\0';
                        t.string = buff;
                        token_base[ctp] = t;
                        return &token_base[ctp++];
                    case '\0':
                        printError(L"unexcept end of character flow.");
                    default:
                        *buff_p++ = g_str_buff[ccp - 1];
                        break;
                }
                break;

            case 2: /* handle number */
                if (g_str_buff[ccp] >= '0' && g_str_buff[ccp] <= '9') {
                    *buff_p++ = g_str_buff[ccp++];
                } else if (g_str_buff[ccp] == '.') {
                    *buff_p++ = g_str_buff[ccp++];
                    state = 3;
                } else {
                    t.type = TT_NUM;
                    *buff_p = '\0';
                    t.num = wcstold(buff, NULL);
                    token_base[ctp] = t;
                    return &token_base[ctp++];
                }
                break;

            case 3: /* handle number which has float */
                if (g_str_buff[ccp] >= '0' && g_str_buff[ccp] <= '9') {
                    *buff_p++ = g_str_buff[ccp++];
                } else {
                    t.type = TT_NUM;
                    *buff_p = '\0';
                    t.num = wcstold(buff, NULL);
                    token_base[ctp] = t;
                    return &token_base[ctp++];
                }
                break;

            case 4: /* handle bool value, t'r'ue */
                switch (g_str_buff[ccp++]) {
                    case 'r':
                        state = 41;
                        break;
                    default:
                        printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 2]);
                }
                break;

            case 41:    /* handle bool value, tr'u'e */
                switch (g_str_buff[ccp++]) {
                    case 'u':
                        state = 42;
                        break;
                    default:
                        printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 3]);
                }
                break;

            case 42:    /* handle bool value, tru'e' */
                switch (g_str_buff[ccp++]) {
                    case 'e':
                        state = 43;
                        break;
                    default:
                        printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 4]);
                }
                break;

            case 43:    /* handle bool value, true */
                if ((g_str_buff[ccp] >= 'a' && g_str_buff[ccp] <= 'z') || (g_str_buff[ccp] >= 'A' && g_str_buff[ccp] <= 'Z')) {
                    printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 4]);
                }
                t.type = TT_TRUE;
                token_base[ctp] = t;
                return &token_base[ctp++];

            case 5: /* handle bool value, f'a'lse */
                switch (g_str_buff[ccp++]) {
                    case 'a':
                        state = 51;
                        break;
                    default:
                        printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 2]);
                }
                break;

            case 51:    /* handle bool value, fa'l'se */
                switch (g_str_buff[ccp++]) {
                    case 'l':
                        state = 52;
                        break;
                    default:
                        printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 3]);
                }
                break;

            case 52:    /* handle bool value, fal's'e */
                switch (g_str_buff[ccp++]) {
                    case 's':
                        state = 53;
                        break;
                    default:
                        printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 4]);
                }
                break;

            case 53:    /* handle bool value, fals'e' */
                switch (g_str_buff[ccp++]) {
                    case 'e':
                        state = 54;
                        break;
                    default:
                        printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 5]);
                }
                break;

            case 54:    /* handle bool value, false */
                if ((g_str_buff[ccp] >= 'a' && g_str_buff[ccp] <= 'z') || (g_str_buff[ccp] >= 'A' && g_str_buff[ccp] <= 'Z')) {
                    printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 5]);
                }
                t.type = TT_FALSE;
                token_base[ctp] = t;
                return &token_base[ctp++];

            case 6: /* handle n'u'll */
                switch (g_str_buff[ccp++]) {
                    case 'u':
                        state = 61;
                        break;
                    default:
                        printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 2]);
                }
                break;

            case 61:    /* handle nu'l'l */
                switch (g_str_buff[ccp++]) {
                    case 'l':
                        state = 62;
                        break;
                    default:
                        printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 3]);
                }
                break;

            case 62:    /* handle nul'l' */
                switch (g_str_buff[ccp++]) {
                    case 'l':
                        state = 63;
                        break;
                    default:
                        printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 4]);
                }
                break;

            case 63:    /* handle null */
                if ((g_str_buff[ccp] >= 'a' && g_str_buff[ccp] <= 'z') || (g_str_buff[ccp] >= 'A' && g_str_buff[ccp] <= 'Z')) {
                    printError(L"unexcept character \'%lc\'.", g_str_buff[ccp - 4]);
                }
                t.type = TT_NULL;
                token_base[ctp] = t;
                return &token_base[ctp++];
        }
    }
}

wchar_t * getTokenTypeName(json_token_type type) {
    switch (type) {
        case TT_NUM: return L"NUMBER";
        case TT_STRING: return L"STRING";
        case TT_RBRACKET: return L"]";
        case TT_LBRACKET: return L"[";
        case TT_RBRACE: return L"}";
        case TT_LBRACE: return L"{";
        case TT_TRUE: return L"TRUE";
        case TT_FALSE: return L"FALSE";
        case TT_COLON: return L":";
        case TT_COMMA: return L",";
        case TT_NULL: return L"NULL";
        case TERMINATOR: return L"EOF";
        case TT_ANY: return L"ANY";
    }
}

json_token * matchNext(json_token_type type) {
    int i;
    json_token *t = next();

    if ((type & t->type) == t->type) {
        return t;
    }

    fwprintf(stderr, L"parse error at line %d, column %d: unexcept token, except ", cl + 1, cc);
    if ((type & 0) == 0) {
        fwprintf(stderr, L"\'%ls\', ", getTokenTypeName(0));
    }

    for(i = 1; i <= 0x400 /* 0100 0000 0000 B */; i <<= 1) {
        if ((type & i) == i) {
            fwprintf(stderr, L"\'%ls\', ", getTokenTypeName(i));
        }
    }

    fwprintf(stderr, L"parse has been terminated.\n");
    terminate(1);
    /* could not run into here */
    return NULL;
}

void rollback() {
    ctp -= 1;
    if (ctp < 0) {
        fwprintf(stderr, L"internal error, rollback operation at the first token.\n");
        terminate(1);
    }
}

void terminate(int exitcode) {
    destroyParser();
    exit(exitcode);
}