CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -I./src

THREADFLAGS = -lpthread

SRC = src/udp_client.c
BUILD_DIR = build/
TARGET = udp_chat

all: build

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

build: $(BUILD_DIR) $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(SRC) -o $(BUILD_DIR)$(TARGET) $(THREADFLAGS)

run:
	./$(BUILD_DIR)$(TARGET)

clean:
	rm -f $(BUILD_DIR)$(TARGET)
	rm -f *.txt
