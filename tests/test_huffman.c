#include <assert.h>
#include <stdio.h>
#include "huffman.h"

// ============================================
// Test: Encode and decode simple inputs
// ============================================
void test_round_trip(void) {
    const unsigned char binary[] = {0, 255, 0, 128, 1};
    const unsigned char *inputs[] = {
        (const unsigned char *)"", (const unsigned char *)"aaaaa",
        (const unsigned char *)"hello huffman", binary
    };
    const size_t sizes[] = {0, 5, 13, sizeof(binary)};

    for (size_t i = 0; i < 4; ++i) {
        FILE *input = tmpfile();
        FILE *encoded = tmpfile();
        FILE *decoded = tmpfile();
        assert(input != NULL && encoded != NULL && decoded != NULL);
        assert(fwrite(inputs[i], 1, sizes[i], input) == sizes[i]);
        rewind(input);
        assert(huffman_encode(input, encoded) == EXIT_SUCCESS);
        rewind(encoded);
        assert(huffman_decode(encoded, decoded) == EXIT_SUCCESS);
        rewind(decoded);
        for (size_t j = 0; j < sizes[i]; ++j) {
            assert(fgetc(decoded) == inputs[i][j]);
        }
        assert(fgetc(decoded) == EOF);
        fclose(input);
        fclose(encoded);
        fclose(decoded);
    }
}


// ============================================
// Test: Reject invalid streams
// ============================================
void test_invalid_streams(void) {
    FILE *stream = tmpfile();
    assert(stream != NULL);
    assert(huffman_encode(NULL, stream) == EXIT_FAILURE);
    assert(huffman_encode(stream, NULL) == EXIT_FAILURE);
    assert(huffman_encode(stream, stream) == EXIT_FAILURE);
    assert(huffman_decode(NULL, stream) == EXIT_FAILURE);
    assert(huffman_decode(stream, NULL) == EXIT_FAILURE);
    assert(huffman_decode(stream, stream) == EXIT_FAILURE);
    fclose(stream);
}


// ============================================
// Test: Reject a truncated header
// ============================================
void test_truncated_header(void) {
    FILE *input = tmpfile();
    FILE *output = tmpfile();
    assert(input != NULL && output != NULL);
    assert(fputs("short", input) >= 0);
    rewind(input);
    assert(huffman_decode(input, output) == EXIT_FAILURE);
    fclose(input);
    fclose(output);
}


// ============================================
// Main test runner
// ============================================
int main(void) {
    printf("Running huffman tests...\n");

    test_round_trip();
    test_invalid_streams();
    test_truncated_header();

    printf("All tests passed!\n");
    return 0;
}
