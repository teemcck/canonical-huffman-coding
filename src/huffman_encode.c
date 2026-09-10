#include "huffman.h"
#include "huffman_tree.h"
#include "canonical.h"
#include "bitio.h"

int huffman_encode(FILE *input, FILE *output) {
    if (input == NULL || output == NULL || input == output) {
        return EXIT_FAILURE;
    }

    // Save the starting position so the input can be read twice.
    fpos_t start_position;
    if (fgetpos(input, &start_position) != 0 || fsetpos(input, &start_position) != 0) {
        return EXIT_FAILURE;
    }

    // Create a frequencies array indexed by each possible symbol.
    uint64_t frequencies[SYMBOL_COUNT] = {0};
    uint32_t total_symbols = 0;
    int symbol;

    // Read bytes until EOF, counting each symbol and the total input size.
    while ((symbol = fgetc(input)) != EOF) {
        if (total_symbols == UINT32_MAX) {
            return EXIT_FAILURE;
        }
        ++frequencies[symbol];
        ++total_symbols;
    }

    if (ferror(input)) {
        return EXIT_FAILURE;
    }

    // Build a min heap and merge the two smallest nodes until one tree remains.
    node *root = NULL;
    int status = build_huffman_tree(frequencies, &root);
    if (status != EXIT_SUCCESS) {
        return status;
    }

    // Create a lengths array indexed by symbol and build lengths from the tree.
    uint8_t lengths[SYMBOL_COUNT] = {0};
    build_code_lengths(root, lengths, 0);

    // Release the tree once its code lengths have been extracted.
    release_tree(root);

    // Count how many symbols have each code length for the future limiter.
    size_t length_counts[SYMBOL_COUNT] = {0};
    for (size_t index = 0; index < SYMBOL_COUNT; ++index) {
        if (lengths[index] != 0) {
            ++length_counts[lengths[index]];
        }
    }

    // Adjust the histogram and per-symbol lengths here before building codes.
    status = limit_code_lengths(frequencies, lengths, length_counts);
    if (status != EXIT_SUCCESS) {
        return status;
    }

    // Check that the final lengths form a valid, supported prefix code.
    size_t counts[MAX_CODE_LEN + 1];
    status = validate_code_lengths(lengths, counts);
    if (status != EXIT_SUCCESS) {
        return status;
    }

    // Build a sorted array of symbol, length pairs.
    symbol_length_pair pairs[SYMBOL_COUNT];
    size_t code_count = build_sorted_symbol_length_pairs(pairs, lengths);

    // Build canonical codes from the sorted symbol lengths.
    huffman_code codebook[SYMBOL_COUNT];
    build_canonical_codes(codebook, pairs, code_count);

    // Reset the input stream to where frequency counting began.
    if (fsetpos(input, &start_position) != 0) {
        return EXIT_FAILURE;
    }

    // Write code lengths and the total symbol count to the header.
    // The count always uses four little-endian bytes, on every machine.
    if (fwrite(lengths, 1, sizeof(lengths), output) != sizeof(lengths)) {
        return EXIT_FAILURE;
    }

    for (unsigned byte = 0; byte < 4; ++byte) {
        if (fputc((int)((total_symbols >> (8 * byte)) & 255), output) == EOF) {
            return EXIT_FAILURE;
        }
    }

    // The bit writer packs variable-length codes into output bytes.
    bit_writer writer;
    bit_writer_init(&writer, output);

    // Read each symbol again and write its translated canonical code.
    for (uint32_t index = 0; index < total_symbols; ++index) {
        symbol = fgetc(input);
        if (symbol == EOF || codebook[symbol].length == 0) {
            return EXIT_FAILURE;
        }

        huffman_code code = codebook[symbol];
        if (bit_writer_write_bits(&writer, code.value, code.length) == EOF) {
            return EXIT_FAILURE;
        }
    }

    // Write any remaining bits with zero padding, then flush the output stream.
    if (bit_writer_flush(&writer) == EOF || fflush(output) == EOF) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
