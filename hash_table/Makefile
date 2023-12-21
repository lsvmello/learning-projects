.PHONY: clean test

CFLAGS = -Wall -Werror -Wextra -g
LIBFILES = src/hash_table.c src/prime.c
TESTFILES = test/hash_table_test.c

build: $(LIBFILES)
	$(CC) $(CFLAGS) -shared -o build/hash_table.so -fPIC $(LIBFILES)

build/hash_table_test: $(TESTFILES) $(LIBFILES)
	$(CC) $(CFLAGS) -o build/hash_table_test $(LIBFILES) $(TESTFILES) -lm

test: build/hash_table_test
	./build/hash_table_test

clean:
	rm -rf build/
	mkdir -p build/
