CC = gcc
CFLAGS = -Wall -Wextra -Werror -I./src
LDFLAGS = -lcriterion

SRC = src/contacts.c src/console_input.c src/main.c
TEST_TARGET = test_contacts.c
TESTS = tests/test_contacts.c
TESTSFLAGS = -DCRITERION_TESTING
TARGET = contacts
TEST_TARGET = test_contacts

all: test

build:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
run:
	./$(TARGET)
test:
	$(CC) $(CFLAGS) $(TESTSFLAGS) $(TESTS) $(SRC) $(LDFLAGS) -o $(TEST_TARGET)
	./$(TEST_TARGET) --verbose --color=always

clean:
	rm -f $(TARGET)