#include "huffman.h"
#include "canonical.h"
#include "bitio.h"
#include "huffman_header.h"

// This function is inspired by Mark Adler's implementation:
// https://github.com/madler/zlib/blob/master/contrib/puff/puff.c
// Return success when a complete code is found and write its symbol to *symbol.
static int decode_next_symbol(bit_reader *reader,
                              const symbol_length_pair *sorted_symbols,
                              const size_t *codes_per_length, uint8_t *symbol) {
    unsigned code = 0;
    unsigned first_code = 0;
    size_t first_index = 0;

    // Read one bit at a time until the code falls in a canonical length group.
    for (size_t length = 1; length <= MAX_CODE_LEN; ++length) {
        int bit = bit_reader_read_bit(reader);
        if (bit == EOF) {
            return EXIT_FAILURE;
        }

        code = (code << 1) | (unsigned)bit;
        if (code >= first_code && code - first_code < codes_per_length[length]) {
            // Convert the position in the sorted table back to the original byte.
            *symbol = sorted_symbols[first_index + code - first_code].symbol;
            return EXIT_SUCCESS;
        }

        // Move to the next group, whose codes have one more bit.
        first_index += codes_per_length[length];
        first_code = (first_code + (unsigned)codes_per_length[length]) << 1;
    }

    return EXIT_FAILURE;
}

int huffman_decode(FILE *input, FILE *output) {
    if (input == NULL || output == NULL || input == output) {
        return EXIT_FAILURE;
    }

    uint8_t code_lengths[SYMBOL_COUNT];
    uint32_t total_symbols;
    if (read_header(input, code_lengths, &total_symbols) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    // Validate the lengths and count the number of codes of each length.
    size_t codes_per_length[MAX_CODE_LEN + 1];
    int status = validate_code_lengths(code_lengths, codes_per_length);
    if (status != EXIT_SUCCESS) {
        return status;
    }

    // Sort symbols by code length, then symbol value, for canonical lookup.
    symbol_length_pair sorted_symbols[SYMBOL_COUNT];
    size_t code_count = build_sorted_symbol_length_pairs(sorted_symbols, code_lengths);

    // An empty payload must have an empty table; each listed symbol needs a byte.
    if ((code_count == 0) != (total_symbols == 0) || code_count > total_symbols) {
        return EXIT_FAILURE;
    }

    // Read the encoded payload one bit at a time.
    bit_reader reader;
    bit_reader_init(&reader, input);

    // Pull bits until a symbol is found, then write that byte to the output.
    for (uint32_t index = 0; index < total_symbols; ++index) {
        uint8_t symbol;
        status = decode_next_symbol(&reader, sorted_symbols, codes_per_length, &symbol);
        if (status != EXIT_SUCCESS) {
            return status;
        }

        if (fputc(symbol, output) == EOF) {
            return EXIT_FAILURE;
        }
    }

    // Only zero padding to the byte boundary is allowed after the payload.
    if (reader.next_bit < 8 && (reader.buffer >> reader.next_bit) != 0) {
        return EXIT_FAILURE;
    }

    if (fgetc(input) != EOF) {
        return EXIT_FAILURE;
    }

    if (ferror(input) || fflush(output) == EOF) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
