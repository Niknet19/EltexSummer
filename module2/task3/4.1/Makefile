CC = gcc
CFLAGS = -g -Wall -Wextra -I./src -I./ui -I./list
LDFLAGS = -lcriterion

SRC = src/list/list.c src/main.c src/rand.c src/contacts.c src/ui/console_input.c
TESTS = tests/list_tests.c
TESTSFLAGS = -DCRITERION_TESTING
TARGET = list_bin
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