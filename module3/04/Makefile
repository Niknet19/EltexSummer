CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -I./src 

SRC = src/interpreter.c
BUILD_DIR = build/
TARGET = interpreter

all: build

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

build: $(BUILD_DIR) $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(SRC) -o $(BUILD_DIR)$(TARGET)

run:
	./$(BUILD_DIR)$(TARGET)

clean:
	rm -f $(BUILD_DIR)$(TARGET)