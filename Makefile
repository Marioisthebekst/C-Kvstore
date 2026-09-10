CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude

all: src/db.c src/repl.c src/main.c
	$(CC) $(CFLAGS) src/db.c src/repl.c src/main.c -o kv_store

test: src/db.c src/repl.c test/test.c
	$(CC) $(CFLAGS) src/db.c src/repl.c test/test.c -o test_runner
	./test_runner

clean:
	rm -f kv_store test_runner *.exe *.o