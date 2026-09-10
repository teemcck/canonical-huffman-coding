#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>

// Return EXIT_SUCCESS or EXIT_FAILURE. The caller owns and closes both streams.
// Encoding starts at the current input position and requires seekable input.
int huffman_encode(FILE *input, FILE *output);
int huffman_decode(FILE *input, FILE *output);

#endif
