#include <assert.h>
#include <stdio.h>
#include "minheap.h"

// ============================================
// Test: Pop nodes in frequency order
// ============================================
void test_heap_order(void) {
    minheap heap = {0};
    huffman_node entries[] = {{.frequency = 5}, {.frequency = 1},
                      {.frequency = 3}, {.frequency = 1}};
    assert(heap_pop_top(&heap) == NULL);
    for (size_t i = 0; i < 4; ++i) {
        assert(heap_insert(&heap, &entries[i]) == 0);
    }
    assert(heap.size == 4);
    const uint64_t expected[] = {1, 1, 3, 5};
    for (size_t i = 0; i < 4; ++i) {
        huffman_node *entry = heap_pop_top(&heap);
        assert(entry != NULL);
        assert(entry->frequency == expected[i]);
    }
    assert(heap.size == 0);
    assert(heap_pop_top(&heap) == NULL);
}


// ============================================
// Test: Reject insertion into a full heap
// ============================================
void test_full_heap(void) {
    minheap heap = {0};
    huffman_node entries[SYMBOL_COUNT] = {0};
    huffman_node extra = {.frequency = 1};
    for (size_t i = 0; i < SYMBOL_COUNT; ++i) {
        entries[i].frequency = i;
        assert(heap_insert(&heap, &entries[i]) == 0);
    }
    assert(heap_insert(&heap, &extra) == -1);
    assert(heap.size == SYMBOL_COUNT);
}


// ============================================
// Test: Insert between pops
// ============================================
void test_interleaved_operations(void) {
    minheap heap = {0};
    huffman_node entries[] = {{.frequency = 8}, {.frequency = 0},
                      {.frequency = 5}, {.frequency = 2}};
    assert(heap_insert(&heap, &entries[0]) == 0);
    assert(heap_insert(&heap, &entries[1]) == 0);
    assert(heap_pop_top(&heap) == &entries[1]);
    assert(heap_insert(&heap, &entries[2]) == 0);
    assert(heap_insert(&heap, &entries[3]) == 0);
    assert(heap_pop_top(&heap) == &entries[3]);
    assert(heap_pop_top(&heap) == &entries[2]);
    assert(heap_pop_top(&heap) == &entries[0]);
    assert(heap.size == 0);
}


// ============================================
// Main test runner
// ============================================
int main(void) {
    printf("Running minheap tests...\n");

    test_heap_order();
    test_full_heap();

    test_interleaved_operations();

    printf("All tests passed!\n");
    return 0;
}
