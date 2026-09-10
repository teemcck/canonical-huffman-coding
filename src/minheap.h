#ifndef MINHEAP_H
#define MINHEAP_H

#include "huffman_tree.h"

typedef struct {
    node *nodes[SYMBOL_COUNT];
    size_t size;
} minheap;

/* Heap borrows nodes; tree construction owns and frees them. */
int heap_insert(minheap *heap, node *entry);
node *heap_pop_top(minheap *heap);

#endif
