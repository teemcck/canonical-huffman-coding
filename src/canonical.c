#include <stdlib.h>
#include "canonical.h"

// Sort shorter codes first; break ties using the symbol value.
static int compare_symbol_lengths(const void *left, const void *right) {
    const symbol_length_pair *first = left;
    const symbol_length_pair *second = right;

    if (first->length != second->length) {
        return (first->length > second->length) - (first->length < second->length);
    }
    return (first->symbol > second->symbol) - (first->symbol < second->symbol);
}

size_t build_sorted_symbol_length_pairs(symbol_length_pair *pairs, const uint8_t *lengths) {
    size_t code_count = 0;

    // A zero length means the symbol does not occur in the code table.
    for (size_t symbol = 0; symbol < SYMBOL_COUNT; ++symbol) {
        if (lengths[symbol] != 0) {
            pairs[code_count++] = (symbol_length_pair){(uint8_t)symbol, lengths[symbol]};
        }
    }
    qsort(pairs, code_count, sizeof(*pairs), compare_symbol_lengths);

    return code_count;
}

int validate_code_lengths(const uint8_t *lengths, size_t *counts) {
    for (size_t length = 0; length <= MAX_CODE_LEN; ++length) {
        counts[length] = 0;
    }

    size_t code_count = 0;

    for (size_t symbol = 0; symbol < SYMBOL_COUNT; ++symbol) {
        if (lengths[symbol] > MAX_CODE_LEN) {
            return EXIT_FAILURE;
        }
        if (lengths[symbol] != 0) {
            ++counts[lengths[symbol]];
            ++code_count;
        }
    }

    // Each unused prefix has two children at the next tree level.
    // Assigning a code consumes a slot, so it cannot be reused as a prefix.
    size_t available = 1;
    for (size_t length = 1; length <= MAX_CODE_LEN; ++length) {
        available *= 2;
        if (counts[length] > available) {
            return EXIT_FAILURE;
        }
        available -= counts[length];
    }

    // Accept a full tree, or the special cases of empty input and one symbol.
    if (code_count == 0 || (code_count == 1 && counts[1] == 1) || available == 0) {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

// Requires validated pairs sorted by length, then symbol.
void build_canonical_codes(huffman_code *codes, const symbol_length_pair *pairs, size_t code_count) {
    uint32_t current_code = 0;
    uint8_t previous_length = 0;

    for (size_t symbol = 0; symbol < SYMBOL_COUNT; ++symbol) {
        codes[symbol].value = 0;
        codes[symbol].length = 0;
    }

    for (size_t index = 0; index < code_count; ++index) {
        // Append zero bits when moving to a longer code length.
        current_code <<= pairs[index].length - previous_length;

        codes[pairs[index].symbol] = (huffman_code){(uint16_t)current_code, pairs[index].length};
        previous_length = pairs[index].length;
        ++current_code;
    }
}
