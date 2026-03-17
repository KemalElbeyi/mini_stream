CC      = gcc
CFLAGS  = -Wall -Wextra -g
SRC     = src/ministream.c src/bellek_izci.c src/hash_map.c

all: test_temel test_bellek benchmark

test_temel: $(SRC) test/test_temel.c
	$(CC) $(CFLAGS) $(SRC) test/test_temel.c -o test_temel

test_bellek: $(SRC) test/test_bellek.c
	$(CC) $(CFLAGS) $(SRC) test/test_bellek.c -o test_bellek

benchmark: $(SRC) test/benchmark.c
	$(CC) $(CFLAGS) $(SRC) test/benchmark.c -o benchmark

test: test_temel test_bellek
	./test_temel
	./test_bellek

valgrind: test_temel
	valgrind --leak-check=full --show-leak-kinds=all \
	         --track-origins=yes ./test_temel

clean:
	rm -f test_temel test_bellek benchmark