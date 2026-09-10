#ifndef BITIO_H
#define BITIO_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
    FILE *stream;
    uint8_t buffer;
    unsigned bit_pos;
} bit_reader;

typedef struct {
    FILE *stream;
    uint8_t buffer;
    unsigned bit_pos;
} bit_writer;

/* Code bits are emitted MSB first, packed into each byte from bit 0 to bit 7. */
void bit_reader_init(bit_reader *reader, FILE *stream);
int bit_reader_read_bit(bit_reader *reader); /* 0, 1, or EOF; inspect ferror for I/O errors. */
void bit_writer_init(bit_writer *writer, FILE *stream);
int bit_writer_write_bit(bit_writer *writer, int bit);
int bit_writer_write_bits(bit_writer *writer, uint16_t value, uint8_t length);
/* Returns 0 on success, EOF on error; zero-pads and resets the buffer. */
int bit_writer_flush(bit_writer *writer);

#endif
