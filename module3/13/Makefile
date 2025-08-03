CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -I./src

THREADFLAGS = -lpthread

SRC_CLIENT = src/tcp_client.c
SRC_SERVER = src/tcp_server.c
BUILD_DIR = build/
TARGET_CLIENT = tcp_client
TARGET_SERVER = tcp_server

all: build-client build-server

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

build-client: $(BUILD_DIR) $(TARGET_CLIENT)

$(TARGET_CLIENT): $(SRC_CLIENT)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)$@

build-server: $(BUILD_DIR) $(TARGET_SERVER)

$(TARGET_SERVER): $(SRC_SERVER)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)$@

run-server:
	./$(BUILD_DIR)$(TARGET_SERVER) $(filter-out $@,$(MAKECMDGOALS))

run-client:
	./$(BUILD_DIR)$(TARGET_CLIENT) $(filter-out $@,$(MAKECMDGOALS))

clean:
	rm -f $(BUILD_DIR)$(TARGET_CLIENT)
	rm -f $(BUILD_DIR)$(TARGET_SERVER)
	rm -f *.txt

%:
    @:
