CC = gcc
CFLAGS = -g -Wall -Wextra -I./src 
LDFLAGS = -lcriterion

SRC = src/avltree.c src/main.c src/contacts.c src/console_input.c
TESTS = tests/avltree_tests.c
TESTSFLAGS = -DCRITERION_TESTING
TARGET = avltree_bin
TEST_TARGET = test_list

all: build

build:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
run:
	./$(TARGET)
test:
	$(CC) $(CFLAGS) $(TESTSFLAGS) $(TESTS) $(SRC) $(LDFLAGS) -o $(TEST_TARGET)
	./$(TEST_TARGET) --verbose --color=always

clean:
	rm -f $(TARGET) $(TEST_TARGET)