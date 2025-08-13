CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -I./src

THREADFLAGS = -lpthread

SRC_CLIENT = src/client.c
SRC_SERVER = src/server.c
BUILD_DIR = build/
TARGET_CLIENT = client
TARGET_SERVER = server

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
	sudo ./$(BUILD_DIR)$(TARGET_SERVER) $(filter-out $@,$(MAKECMDGOALS))

run-client:
	sudo ./$(BUILD_DIR)$(TARGET_CLIENT) $(filter-out $@,$(MAKECMDGOALS))

clean:
	rm -f $(BUILD_DIR)$(TARGET_CLIENT)
	rm -f $(BUILD_DIR)$(TARGET_SERVER)
	rm -f *.txt

%:
    @:
