#include "minheap.h"

static void swap_nodes(minheap *heap, size_t first, size_t second) {
    huffman_node *temporary = heap->nodes[first];
    heap->nodes[first] = heap->nodes[second];
    heap->nodes[second] = temporary;
}

int heap_insert(minheap *heap, huffman_node *entry) {
    if (heap->size == SYMBOL_COUNT) {
        return -1;
    }

    // Insert a node as a leaf, then move it up until the heap order is restored.
    size_t index = heap->size++;
    heap->nodes[index] = entry;
    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (heap->nodes[parent]->frequency <= heap->nodes[index]->frequency) {
            break;
        }
        swap_nodes(heap, parent, index);
        index = parent;
    }

    return 0;
}

huffman_node *heap_pop_top(minheap *heap) {
    if (heap->size == 0) {
        return NULL;
    }

    // Replace the smallest node with the last leaf and move it down as needed.
    huffman_node *result = heap->nodes[0];
    heap->nodes[0] = heap->nodes[--heap->size];
    size_t index = 0;
    while (2 * index + 1 < heap->size) {
        size_t child = 2 * index + 1;
        if (child + 1 < heap->size &&
            heap->nodes[child + 1]->frequency < heap->nodes[child]->frequency) {
            ++child;
        }
        if (heap->nodes[index]->frequency <= heap->nodes[child]->frequency) {
            break;
        }
        swap_nodes(heap, index, child);
        index = child;
    }

    return result;
}
