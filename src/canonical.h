#ifndef CANONICAL_H
#define CANONICAL_H

#include <stddef.h>
#include <stdint.h>
#include "huffman.h"

#define SYMBOL_COUNT 256
#define MAX_CODE_LEN 15

typedef struct {
    uint8_t symbol;
    uint8_t length;
} symbol_length_pair;

typedef struct {
    uint16_t value;
    uint8_t length;
} huffman_code;

size_t build_sorted_symbol_length_pairs(symbol_length_pair *pairs, const uint8_t *lengths);
// Return EXIT_FAILURE for unsupported or invalid code lengths.
int validate_code_lengths(const uint8_t *lengths, size_t *counts);
/* Requires validated lengths sorted by length, then symbol. */
void build_canonical_codes(huffman_code *codes, const symbol_length_pair *pairs, size_t code_count);

#endif
