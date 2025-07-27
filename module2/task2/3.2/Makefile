CC = gcc
CFLAGS = -Wall -Wextra -Werror -I./src
LDFLAGS = -lcriterion

SRC = src/ipcalc.c
TESTS = tests/test_ipcalc.c
TESTSFLAGS = -DCRITERION_TESTING
TARGET = ipcalc
TEST_TARGET = test_ipcalc

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