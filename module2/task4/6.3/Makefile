CC = gcc
CFLAGS = -fPIC -Wall -Wextra
LDFLAGS = -ldl -lm
SHARED_FLAGS = -shared

TARGET = main
LIBS = libsum.so libsub.so libmul.so libdiv.so

SRC_DIR = src

.PHONY: all clean

all: $(TARGET) $(LIBS)

libsum.so: sum.o
	$(CC) $(SHARED_FLAGS) $< -o $@

libsub.so: sub.o
	$(CC) $(SHARED_FLAGS) $< -o $@

libmul.so: mul.o
	$(CC) $(SHARED_FLAGS) $< -o $@

libdiv.so: div.o
	$(CC) $(SHARED_FLAGS) $< -o $@

$(TARGET): $(SRC_DIR)/main.c $(LIBS)
	$(CC) -o $@ $< $(LDFLAGS)

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(TARGET)
	
clean:
	rm -f $(TARGET) $(LIBS) *.o

