#include "bitio.h"

void bit_reader_init(bit_reader *reader, FILE *stream) {
    *reader = (bit_reader){stream, 0, 8};
}

int bit_reader_read_bit(bit_reader *reader) {
    if (reader->stream == NULL) {
        return EOF;
    }

    // Refill only after all eight buffered bits have been consumed.
    if (reader->next_bit == 8) {
        int byte = fgetc(reader->stream);
        if (byte == EOF) {
            return EOF;
        }
        reader->buffer = (uint8_t)byte;
        reader->next_bit = 0;
    }

    return (reader->buffer >> reader->next_bit++) & 1;
}

void bit_writer_init(bit_writer *writer, FILE *stream) {
    *writer = (bit_writer){stream, 0, 0};
}

int bit_writer_write_bit(bit_writer *writer, int bit) {
    if (writer->stream == NULL || writer->bits_used >= 8) {
        return EOF;
    }

    writer->buffer |= (uint8_t)((bit & 1) << writer->bits_used++);
    return writer->bits_used == 8 ? bit_writer_flush(writer) : 0;
}

int bit_writer_write_bits(bit_writer *writer, uint16_t bits, uint8_t length) {
    if (length > 16) {
        return EOF;
    }

    // Emit the code from its highest used bit down to bit zero.
    for (unsigned remaining = length; remaining > 0; --remaining) {
        if (bit_writer_write_bit(writer, (bits >> (remaining - 1)) & 1) == EOF) {
            return EOF;
        }
    }

    return 0;
}

int bit_writer_flush(bit_writer *writer) {
    if (writer->stream == NULL) {
        return EOF;
    }

    if (writer->bits_used != 0) {
        if (fputc(writer->buffer, writer->stream) == EOF) {
            return EOF;
        }

        // A successful flush starts a fresh byte; repeated flushes do nothing.
        writer->buffer = 0;
        writer->bits_used = 0;
    }

    return 0;
}
