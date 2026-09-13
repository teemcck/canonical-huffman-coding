#ifndef HUFFMAN_HEADER_H
#define HUFFMAN_HEADER_H

#include "canonical.h"

/* File header: 256 code lengths indexed by symbol, then a four-byte
 * little-endian symbol count. These helpers handle I/O only; callers validate
 * code lengths and their consistency with the count. Streams must be non-null.
 * Return EXIT_SUCCESS or EXIT_FAILURE. */
int read_header(FILE *input, uint8_t code_lengths[SYMBOL_COUNT], uint32_t *total_symbols);
int write_header(FILE *output, const uint8_t code_lengths[SYMBOL_COUNT], uint32_t total_symbols);

#endif
