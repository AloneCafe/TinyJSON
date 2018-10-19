/*
 * internal dynamic memory management
 */

#ifndef TINYJSON_MEM_H
#define TINYJSON_MEM_H

#include <stddef.h>

#define MAX_POINTER_COUNT 100000

void * new(size_t size);

void initMem();
void destroyMem();

#endif //TINYJSON_MEM_H
