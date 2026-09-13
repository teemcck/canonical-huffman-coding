#include <assert.h>
#include <stdio.h>
#include "bitio.h"

// ============================================
// Test: Read bits and reach EOF
// ============================================
void test_read_bits(void) {
    FILE *stream = tmpfile();
    assert(stream != NULL);
    assert(fputc(0x05, stream) != EOF);
    rewind(stream);

    bit_reader reader;
    bit_reader_init(&reader, stream);
    // Bytes are read from the least significant bit first.
    const int expected[] = {1, 0, 1, 0, 0, 0, 0, 0};
    for (size_t i = 0; i < 8; ++i) {
        assert(bit_reader_read_bit(&reader) == expected[i]);
    }
    assert(bit_reader_read_bit(&reader) == EOF);
    fclose(stream);
}


// ============================================
// Test: Write across a byte boundary and flush
// ============================================
void test_write_bits(void) {
    FILE *stream = tmpfile();
    assert(stream != NULL);
    bit_writer writer;
    bit_writer_init(&writer, stream);

    assert(bit_writer_write_bit(&writer, 1) == 0);
    assert(bit_writer_write_bits(&writer, 0x80, 8) == 0);
    assert(bit_writer_flush(&writer) == 0);
    assert(bit_writer_flush(&writer) == 0);
    rewind(stream);
    // The first two bits are 1; the remaining bits and padding are 0.
    assert(fgetc(stream) == 0x03);
    assert(fgetc(stream) == 0x00);
    assert(fgetc(stream) == EOF);
    fclose(stream);
}


// ============================================
// Test: Empty input and invalid arguments
// ============================================
void test_empty_and_invalid(void) {
    FILE *stream = tmpfile();
    assert(stream != NULL);
    bit_reader reader;
    bit_reader_init(&reader, stream);
    assert(bit_reader_read_bit(&reader) == EOF);
    bit_reader_init(&reader, NULL);
    assert(bit_reader_read_bit(&reader) == EOF);

    bit_writer writer;
    bit_writer_init(&writer, stream);
    assert(bit_writer_write_bits(&writer, 0, 0) == 0);
    assert(bit_writer_write_bits(&writer, 0, 17) == EOF);
    assert(bit_writer_flush(&writer) == 0);
    rewind(stream);
    assert(fgetc(stream) == EOF);
    bit_writer_init(&writer, NULL);
    assert(bit_writer_write_bit(&writer, 1) == EOF);
    assert(bit_writer_flush(&writer) == EOF);
    fclose(stream);
}


// ============================================
// Test: Multi-bit codes have a known byte layout
// ============================================
void test_code_bit_order(void) {
    FILE *stream = tmpfile();
    assert(stream != NULL);
    bit_writer writer;
    bit_writer_init(&writer, stream);
    // 110 followed by 10011 makes 11010011, packed as byte 0xCB.
    assert(bit_writer_write_bits(&writer, 6, 3) == 0);
    assert(bit_writer_write_bits(&writer, 19, 5) == 0);
    // Only the lowest three bits of the value are used: 101.
    assert(bit_writer_write_bits(&writer, 0xFFFD, 3) == 0);
    assert(bit_writer_flush(&writer) == 0);
    rewind(stream);
    assert(fgetc(stream) == 0xCB);
    assert(fgetc(stream) == 0x05);
    assert(fgetc(stream) == EOF);
    fclose(stream);
}


// ============================================
// Test: Flush resets the buffer before another write
// ============================================
void test_flush_and_continue(void) {
    FILE *stream = tmpfile();
    assert(stream != NULL);
    bit_writer writer;
    bit_writer_init(&writer, stream);
    assert(bit_writer_write_bits(&writer, 7, 3) == 0);
    assert(bit_writer_flush(&writer) == 0);
    assert(writer.buffer == 0 && writer.bits_used == 0);
    assert(bit_writer_flush(&writer) == 0);
    assert(bit_writer_write_bits(&writer, 0, 2) == 0);
    assert(bit_writer_flush(&writer) == 0);
    rewind(stream);
    assert(fgetc(stream) == 0x07);
    assert(fgetc(stream) == 0x00);
    assert(fgetc(stream) == EOF);
    fclose(stream);
}


// ============================================
// Test: Invalid lengths preserve pending bits
// ============================================
void test_invalid_length_preserves_buffer(void) {
    FILE *stream = tmpfile();
    assert(stream != NULL);
    bit_writer writer;
    bit_writer_init(&writer, stream);
    assert(bit_writer_write_bits(&writer, 5, 3) == 0);
    assert(bit_writer_write_bits(&writer, 0xFFFF, 17) == EOF);
    assert(bit_writer_write_bits(&writer, 0xFFFF, 255) == EOF);
    assert(bit_writer_write_bits(&writer, 0xFFFF, 0) == 0);
    assert(writer.buffer == 5 && writer.bits_used == 3);
    assert(bit_writer_write_bit(&writer, 1) == 0);
    assert(bit_writer_flush(&writer) == 0);
    rewind(stream);
    assert(fgetc(stream) == 0x0D);
    assert(fgetc(stream) == EOF);
    fclose(stream);
}


// ============================================
// Main test runner
// ============================================
int main(void) {
    printf("Running bitio tests...\n");

    test_read_bits();
    test_write_bits();
    test_empty_and_invalid();
    test_code_bit_order();
    test_flush_and_continue();
    test_invalid_length_preserves_buffer();

    printf("All tests passed!\n");
    return 0;
}
