CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude

test: src/db.c src/repl.c tests/test.c tests/testMain.c
	$(CC) $(CFLAGS) -Itests src/db.c src/repl.c tests/test.c tests/testMain.c -o test_runner.exe

all: src/db.c src/repl.c src/main.c
	$(CC) $(CFLAGS) src/db.c src/repl.c src/main.c -o kv_store.exe

clean:
	rm -f *.exe *.o