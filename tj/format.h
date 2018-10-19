/*
 * the json text format functions
 */

#include "parse.h"

#ifndef TINYJSON_FORMAT_H
#define TINYJSON_FORMAT_H

void makeIndent();
void printNode(json_node *node);
void printNodeArray(int len, struct _json_node **node);
void printPairArray(int len, struct _json_pair **pair);

#endif //TINYJSON_FORMAT_H