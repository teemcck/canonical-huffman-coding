#include "bitio.h"

void bit_reader_init(bit_reader *reader, FILE *stream) {
    *reader = (bit_reader){stream, 0, 8};
}

int bit_reader_read_bit(bit_reader *reader) {
    if (reader->stream == NULL) {
        return EOF;
    }

    // Refill only after all eight buffered bits have been consumed.
    if (reader->bit_pos == 8) {
        int byte = fgetc(reader->stream);
        if (byte == EOF) {
            return EOF;
        }
        reader->buffer = (uint8_t)byte;
        reader->bit_pos = 0;
    }

    return (reader->buffer >> reader->bit_pos++) & 1;
}

void bit_writer_init(bit_writer *writer, FILE *stream) {
    *writer = (bit_writer){stream, 0, 0};
}

int bit_writer_write_bit(bit_writer *writer, int bit) {
    if (writer->stream == NULL || writer->bit_pos >= 8) {
        return EOF;
    }

    writer->buffer |= (uint8_t)((bit & 1) << writer->bit_pos++);
    return writer->bit_pos == 8 ? bit_writer_flush(writer) : 0;
}

int bit_writer_write_bits(bit_writer *writer, uint16_t value, uint8_t length) {
    if (length > 16) {
        return EOF;
    }

    // Emit the code from its highest used bit down to bit zero.
    for (unsigned remaining = length; remaining > 0; --remaining) {
        if (bit_writer_write_bit(writer, (value >> (remaining - 1)) & 1) == EOF) {
            return EOF;
        }
    }

    return 0;
}

int bit_writer_flush(bit_writer *writer) {
    if (writer->stream == NULL) {
        return EOF;
    }

    if (writer->bit_pos != 0) {
        if (fputc(writer->buffer, writer->stream) == EOF) {
            return EOF;
        }

        // A successful flush starts a fresh byte; repeated flushes do nothing.
        writer->buffer = 0;
        writer->bit_pos = 0;
    }

    return 0;
}
