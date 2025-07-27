CC = gcc
CFLAGS = -Wall -Wextra -Werror -I./src
LDFLAGS = -lcriterion

SRC = src/simplecalc.c src/simplecalc.h
TESTS = tests/test_calculator.c
TESTSFLAGS = -DCRITERION_TESTING
TARGET = calc
TEST_TARGET = test_calculator

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