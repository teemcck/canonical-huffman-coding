CC = cc
CPPFLAGS = -Isrc
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -O2
SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
TESTS = tests/test_bitio tests/test_canonical tests/test_minheap tests/test_huffman_tree tests/test_huffman

.PHONY: all debug test clean
all: huffman

huffman: $(SOURCES) $(HEADERS) Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) $(LDFLAGS) -o $@

debug:
	$(MAKE) huffman CFLAGS="-std=c17 -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined -fno-pie" LDFLAGS="-fsanitize=address,undefined -no-pie" -B

test: $(TESTS)
	@set -e; for test in $(TESTS); do ./$$test; done

$(TESTS): tests/test_%: tests/test_%.c $(filter-out src/main.c,$(SOURCES)) $(HEADERS) Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(filter-out src/main.c,$(SOURCES)) $(LDFLAGS) -o $@

clean:
	rm -f huffman $(TESTS)
