CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -I./src
LDFLAGS = -lcriterion

SRC = src/main.c src/pqueue.c  src/pqueue.h
TESTS = tests/queue_test.c
TESTSFLAGS = -DCRITERION_TESTING
TARGET = queue
TEST_TARGET = queue_test

all: build

build:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
run:
	./$(TARGET)
test:
	$(CC) $(CFLAGS) $(TESTSFLAGS) $(TESTS) $(SRC) $(LDFLAGS) -o $(TEST_TARGET)
	./$(TEST_TARGET) --verbose --color=always

clean:
	rm -f $(TARGET)