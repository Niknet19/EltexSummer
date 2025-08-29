CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = taxi_server

all: $(TARGET)

$(TARGET): server.c
	$(CC) $(CFLAGS) -D _DEFAULT_SOURCE -o  $(TARGET) server.c

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf /tmp/taxi_fifos

.PHONY: all clean