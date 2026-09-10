#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include "canonical.h"

typedef struct node {
    uint64_t frequency;
    struct node *left;
    struct node *right;
    uint8_t symbol;
} node;

int build_huffman_tree(const uint64_t *frequencies, node **root);
void release_tree(node *root);
void build_code_lengths(const node *root, uint8_t *lengths, unsigned depth);
/* Extension point: adjust histogram, then reassign lengths using frequencies.
 * Histogram index is code length; index zero is unused. */
int limit_code_lengths(const uint64_t *frequencies, uint8_t *lengths,
                                 size_t length_counts[SYMBOL_COUNT]);

#endif
