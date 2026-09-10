#include <stdlib.h>
#include "huffman_tree.h"
#include "minheap.h"

void release_tree(node *root) {
    if (root != NULL) {
        // Free children first, while their pointers are still available.
        release_tree(root->left);
        release_tree(root->right);
        free(root);
    }
}

// Each heap entry owns a separate tree. Free all of them if allocation fails.
static void release_heap_trees(minheap *heap) {
    while (heap->size != 0) {
        release_tree(heap_pop_top(heap));
    }
}

int build_huffman_tree(const uint64_t *frequencies, node **root) {
    minheap heap = {0};
    // Leave the caller with no tree on failure or empty input.
    *root = NULL;

    // Insert one leaf for each symbol that occurs in the input.
    for (size_t symbol = 0; symbol < SYMBOL_COUNT; ++symbol) {
        if (frequencies[symbol] != 0) {
            // calloc initializes the child pointers to null on this platform.
            node *leaf = calloc(1, sizeof(*leaf));
            if (leaf == NULL) {
                release_heap_trees(&heap);
                return EXIT_FAILURE;
            }
            leaf->frequency = frequencies[symbol];
            leaf->symbol = (uint8_t)symbol;
            // At most 256 leaves are inserted into a 256-entry heap.
            heap_insert(&heap, leaf);
        }
    }

    // Repeatedly combine the two least frequent trees into one parent.
    while (heap.size > 1) {
        // Allocate before removing children so failure cleanup can find them.
        node *parent = calloc(1, sizeof(*parent));
        if (parent == NULL) {
            release_heap_trees(&heap);
            return EXIT_FAILURE;
        }
        parent->left = heap_pop_top(&heap);
        parent->right = heap_pop_top(&heap);
        // Internal nodes represent the combined frequency of their children.
        parent->frequency = parent->left->frequency + parent->right->frequency;
        heap_insert(&heap, parent);
    }

    // The remaining entry is the complete tree; an empty heap returns null.
    *root = heap_pop_top(&heap);
    return EXIT_SUCCESS;
}

void build_code_lengths(const node *root, uint8_t *lengths, unsigned depth) {
    if (root == NULL) {
        return;
    }

    // Leaves hold symbols; internal nodes always have two children.
    if (root->left == NULL) {
        // Tree depth gives the bit count. A lone symbol still needs one bit.
        lengths[root->symbol] = (uint8_t)(depth == 0 ? 1 : depth);
        return;
    }

    // Each branch adds one bit to the codes below it.
    build_code_lengths(root->left, lengths, depth + 1);
    build_code_lengths(root->right, lengths, depth + 1);
}

int limit_code_lengths(const uint64_t *frequencies, uint8_t *lengths,
                       size_t length_counts[SYMBOL_COUNT]) {
    /* TODO: implement length limiting here. Update both length_counts and the
     * per-symbol lengths; the encoder validates lengths before writing a header. */
    // These parameters are reserved for your future length-limiting algorithm.
    (void)frequencies;
    (void)lengths;

    // Until limiting is implemented, reject trees that need longer codes.
    for (size_t length = MAX_CODE_LEN + 1; length < SYMBOL_COUNT; ++length) {
        if (length_counts[length] != 0) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
