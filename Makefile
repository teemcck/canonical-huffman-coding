CC = cc
CPPFLAGS = -Isrc
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -O2
SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)

.PHONY: all debug test clean
all: huffman

huffman: $(SOURCES) $(HEADERS) Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) $(LDFLAGS) -o $@

debug:
	$(MAKE) huffman CFLAGS="-std=c17 -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined -fno-pie" LDFLAGS="-fsanitize=address,undefined -no-pie" -B

test:
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_huffman.c $(filter-out src/main.c,$(SOURCES)) $(LDFLAGS) -o tests/test_huffman
	./tests/test_huffman

clean:
	rm -f huffman tests/test_huffman
