#include <wchar.h>
#include "format.h"
#include "parse.h"

int indents = 0;

void printNode(json_node *node) {
    switch (node->type) {
        case NT_PAIR_ARRAY:
            printPairArray(node->array_len, node->pair_array);
            break;

        case NT_NODE_ARRAY:
            printNodeArray(node->array_len, node->node_array);
            break;

        case NT_STRING:
            printf("\"%ls\"\n", node->string);
            break;

        case NT_TRUE:
            printf("%ls\n", L"true");
            break;

        case NT_FALSE:
            printf("%ls\n", L"false");
            break;

        case NT_NULL:
            printf("%ls\n", L"null");
            break;

        case NT_NUM:
            printf("%Lf\n", node->num);
            break;
    }
}

void printNodeArray(int len, struct _json_node **node) {
    makeIndent();
    wprintf(L"[");
    indents += 1;

    int i;
    for (i = 0; i < len; i++) {
        makeIndent();
        switch (node[i]->type) {
            case NT_PAIR_ARRAY:
                wprintf(L"\n");
                printPairArray(node[i]->array_len, node[i]->pair_array);
                if (i < len - 1) {
                    makeIndent();
                    wprintf(L",");
                }
                break;

            case NT_NODE_ARRAY:
                wprintf(L"\n");
                printNodeArray(node[i]->array_len, node[i]->node_array);
                if (i < len - 1) {
                    makeIndent();
                    wprintf(L",");
                }
                break;

            case NT_STRING:
                printf("\"%ls\"%ls", node[i]->string, i < len - 1 ? L",\n" : L"\n");
                break;

            case NT_TRUE:
                printf("%ls%ls", L"true", i < len - 1 ? L",\n" : L"\n");
                break;

            case NT_FALSE:
                printf("%ls%ls", L"false", i < len - 1 ? L",\n" : L"\n");
                break;

            case NT_NULL:
                printf("%ls%ls", L"null", i < len - 1 ? L",\n" : L"\n");
                break;

            case NT_NUM:
                printf("%Lf%ls", node[i]->num, i < len - 1 ? L",\n" : L"\n");
                break;
        }
    }


    indents -= 1;
    makeIndent();
    wprintf(L"]\n");

}

void printPairArray(int len, struct _json_pair **pair) {
    makeIndent();
    wprintf(L"{\n");
    indents += 1;

    int i;
    for (i = 0; i < len; i++) {
        makeIndent();
        printf("\"%ls\" : ", pair[i]->key);
        switch (pair[i]->val->type) {
            case NT_PAIR_ARRAY:
                wprintf(L"\n");
                printPairArray(pair[i]->val->array_len, pair[i]->val->pair_array);
                if (i < len - 1) {
                    makeIndent();
                    wprintf(L",");
                }
                break;

            case NT_NODE_ARRAY:
                wprintf(L"\n");
                printNodeArray(pair[i]->val->array_len, pair[i]->val->node_array);
                if (i < len - 1) {
                    makeIndent();
                    wprintf(L",");
                }
                break;

            case NT_STRING:
                printf("\"%ls\"%ls", pair[i]->val->string, i < len - 1 ? L",\n" : L"\n");
                break;

            case NT_TRUE:
                printf("%ls%ls", L"true", i < len - 1 ? L",\n" : L"\n");
                break;

            case NT_FALSE:
                printf("%ls%ls", L"false", i < len - 1 ? L",\n" : L"\n");
                break;

            case NT_NULL:
                printf("%ls%ls", L"null", i < len - 1 ? L",\n" : L"\n");
                break;

            case NT_NUM:
                printf("%Lf%ls", pair[i]->val->num, i < len - 1 ? L",\n" : L"\n");
                break;
        }

    }

    indents -= 1;
    makeIndent();
    wprintf(L"}\n");

}

void makeIndent() {
    int i;
    for (i = 0; i < indents; i++) {
        wprintf(L"\t");
    }
}
