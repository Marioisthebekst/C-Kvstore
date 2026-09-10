CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
TARGET = kv_store
 
SRCS = src/main.c src/db.c src/repl.c
OBJS = $(SRCS:.c=.o)
 
TEST_TARGET = test_runner
TEST_SRCS = src/db.c src/repl.c test/test.c test/testMain.c
 
all: $(TARGET)
 
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)
 
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
 
test: $(TEST_SRCS)
	$(CC) $(CFLAGS) -Itests $(TEST_SRCS) -o $(TEST_TARGET)
	./$(TEST_TARGET)
 
clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe $(TEST_TARGET) $(TEST_TARGET).exe
 