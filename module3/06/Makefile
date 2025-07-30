CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -I./src

SERVER_SRC = src/queue_chat_server.c
SERVER_TARGET = server
SERVER_BUILD = $(BUILD_DIR)$(SERVER_TARGET)

CLIENT_SRC = src/queue_chat_client.c
CLIENT_TARGET = client
CLIENT_BUILD = $(BUILD_DIR)$(CLIENT_TARGET)

BUILD_DIR = build/


all: build_server build_client

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

build_server: $(BUILD_DIR) $(SERVER_BUILD)

build_client: $(BUILD_DIR) $(CLIENT_BUILD)

$(SERVER_BUILD):
	$(CC) $(CFLAGS) $(SERVER_SRC) -o $(SERVER_BUILD)

$(CLIENT_BUILD):
	$(CC) $(CFLAGS) $(CLIENT_SRC) -o $(CLIENT_BUILD)

rebuild: clean build_client build_server

run-server: build_server
	./$(SERVER_BUILD)

run-client: build_client
	./$(CLIENT_BUILD)

clean:
	rm -rf $(BUILD_DIR)