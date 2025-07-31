CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -I./src

SHMFLAGS = -lrt -lpthread

SRC = src/shared_mem_posix.c
BUILD_DIR = build/
TARGET = shmem

all: build

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

build: $(BUILD_DIR) $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(SRC) -o $(BUILD_DIR)$(TARGET) $(SHMFLAGS)

run:
	./$(BUILD_DIR)$(TARGET)

clean:
	rm -f $(BUILD_DIR)$(TARGET)
	rm -f *.txt
