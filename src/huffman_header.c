#include "huffman_header.h"

int read_header(FILE *input, uint8_t code_lengths[SYMBOL_COUNT], uint32_t *total_symbols) {
    if (fread(code_lengths, 1, SYMBOL_COUNT, input) != SYMBOL_COUNT) {
        return EXIT_FAILURE;
    }

    *total_symbols = 0;
    for (unsigned byte = 0; byte < 4; ++byte) {
        int value = fgetc(input);
        if (value == EOF) {
            return EXIT_FAILURE;
        }
        *total_symbols |= (uint32_t)value << (8 * byte);
    }
    return EXIT_SUCCESS;
}

int write_header(FILE *output, const uint8_t code_lengths[SYMBOL_COUNT], uint32_t total_symbols) {
    if (fwrite(code_lengths, 1, SYMBOL_COUNT, output) != SYMBOL_COUNT) {
        return EXIT_FAILURE;
    }

    for (unsigned byte = 0; byte < 4; ++byte) {
        if (fputc((int)((total_symbols >> (8 * byte)) & 255), output) == EOF) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
