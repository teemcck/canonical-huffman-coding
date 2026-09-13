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

size_t build_sorted_symbol_length_pairs(symbol_length_pair *sorted_symbols, const uint8_t *code_lengths) {
    size_t code_count = 0;

    // A zero length means the symbol does not occur in the code table.
    for (size_t symbol = 0; symbol < SYMBOL_COUNT; ++symbol) {
        if (code_lengths[symbol] != 0) {
            sorted_symbols[code_count++] = (symbol_length_pair){(uint8_t)symbol, code_lengths[symbol]};
        }
    }
    qsort(sorted_symbols, code_count, sizeof(*sorted_symbols), compare_symbol_lengths);

    return code_count;
}

int validate_code_lengths(const uint8_t *code_lengths, size_t *codes_per_length) {
    for (size_t length = 0; length <= MAX_CODE_LEN; ++length) {
        codes_per_length[length] = 0;
    }

    size_t code_count = 0;

    for (size_t symbol = 0; symbol < SYMBOL_COUNT; ++symbol) {
        if (code_lengths[symbol] > MAX_CODE_LEN) {
            return EXIT_FAILURE;
        }
        if (code_lengths[symbol] != 0) {
            ++codes_per_length[code_lengths[symbol]];
            ++code_count;
        }
    }

    // Each unused prefix has two children at the next tree level.
    // Assigning a code consumes a slot, so it cannot be reused as a prefix.
    size_t available = 1;
    for (size_t length = 1; length <= MAX_CODE_LEN; ++length) {
        available *= 2;
        if (codes_per_length[length] > available) {
            return EXIT_FAILURE;
        }
        available -= codes_per_length[length];
    }

    // Accept a full tree, or the special cases of empty input and one symbol.
    if (code_count == 0 || (code_count == 1 && codes_per_length[1] == 1) || available == 0) {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

// Requires validated pairs sorted by length, then symbol.
void build_canonical_codes(huffman_code *codes, const symbol_length_pair *sorted_symbols, size_t code_count) {
    uint32_t current_code = 0;
    uint8_t previous_length = 0;

    for (size_t symbol = 0; symbol < SYMBOL_COUNT; ++symbol) {
        codes[symbol].bits = 0;
        codes[symbol].length = 0;
    }

    for (size_t index = 0; index < code_count; ++index) {
        // Append zero bits when moving to a longer code length.
        current_code <<= sorted_symbols[index].length - previous_length;

        codes[sorted_symbols[index].symbol] = (huffman_code){(uint16_t)current_code, sorted_symbols[index].length};
        previous_length = sorted_symbols[index].length;
        ++current_code;
    }
}
