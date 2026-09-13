#include <assert.h>
#include <stdio.h>
#include "canonical.h"

// ============================================
// Test: Sort by length, then symbol
// ============================================
void test_sorted_pairs(void) {
    uint8_t code_lengths[SYMBOL_COUNT] = {0};
    symbol_length_pair sorted_symbols[SYMBOL_COUNT];
    assert(build_sorted_symbol_length_pairs(sorted_symbols, code_lengths) == 0);
    code_lengths['C'] = 2;
    code_lengths['A'] = 2;
    code_lengths['B'] = 1;
    assert(build_sorted_symbol_length_pairs(sorted_symbols, code_lengths) == 3);
    assert(sorted_symbols[0].symbol == 'B' && sorted_symbols[0].length == 1);
    assert(sorted_symbols[1].symbol == 'A' && sorted_symbols[1].length == 2);
    assert(sorted_symbols[2].symbol == 'C' && sorted_symbols[2].length == 2);
}


// ============================================
// Test: Validate basic code lengths
// ============================================
void test_validate_lengths(void) {
    uint8_t code_lengths[SYMBOL_COUNT] = {0};
    size_t codes_per_length[MAX_CODE_LEN + 1];
    assert(validate_code_lengths(code_lengths, codes_per_length) == EXIT_SUCCESS);
    code_lengths['A'] = 1;
    assert(validate_code_lengths(code_lengths, codes_per_length) == EXIT_SUCCESS);
    code_lengths['B'] = 2;
    code_lengths['C'] = 2;
    assert(validate_code_lengths(code_lengths, codes_per_length) == EXIT_SUCCESS);
    assert(codes_per_length[1] == 1 && codes_per_length[2] == 2);

    code_lengths['C'] = 0; // Incomplete tree.
    assert(validate_code_lengths(code_lengths, codes_per_length) == EXIT_FAILURE);
    code_lengths['B'] = 1;
    code_lengths['C'] = 1; // Too many one-bit codes.
    assert(validate_code_lengths(code_lengths, codes_per_length) == EXIT_FAILURE);
    code_lengths['C'] = MAX_CODE_LEN + 1;
    assert(validate_code_lengths(code_lengths, codes_per_length) == EXIT_FAILURE);
}


// ============================================
// Test: Build known canonical codes
// ============================================
void test_build_codes(void) {
    symbol_length_pair sorted_symbols[] = {{'A', 1}, {'B', 2}, {'C', 2}};
    huffman_code codes[SYMBOL_COUNT];
    build_canonical_codes(codes, sorted_symbols, 3);
    assert(codes['A'].bits == 0 && codes['A'].length == 1);
    assert(codes['B'].bits == 2 && codes['B'].length == 2);
    assert(codes['C'].bits == 3 && codes['C'].length == 2);
    assert(codes['D'].length == 0);

    build_canonical_codes(codes, sorted_symbols, 0);
    assert(codes['A'].bits == 0 && codes['A'].length == 0);
}


// ============================================
// Main test runner
// ============================================
int main(void) {
    printf("Running canonical tests...\n");

    test_sorted_pairs();
    test_validate_lengths();
    test_build_codes();

    printf("All tests passed!\n");
    return 0;
}
