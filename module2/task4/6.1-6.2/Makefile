CC = gcc
CFLAGS = -g -Wall -Wextra -I./src -I./ui -I./list
LDFLAGS = -lcriterion

LIST_SRC = src/list/list.c
LIST_OBJ = $(LIST_SRC:.c=.o)
LIST_LIB_STATIC = liblist.a
LIBFLAGS_STATIC = -L. -llist

LIST_OBJ_PIC = $(LIST_SRC:.c=.pic.o)
LIST_LIB_DYNAMIC = liblist.so
LIBFLAGS_DYNAMIC = -L. -llist -Wl,-rpath,.

MAIN_SRC = src/main.c src/contacts.c src/ui/console_input.c
MAIN_OBJ = $(MAIN_SRC:.c=.o)
MAIN_OBJ_PIC = $(MAIN_SRC:.c=.pic.o)

TESTS = tests/list_tests.c
TESTSFLAGS = -DCRITERION_TESTING

TARGET_STATIC = list_static
TARGET_DYNAMIC = list_dynamic
TEST_TARGET = test_list

all: static
static: lib-static $(TARGET_STATIC)
dynamic: $(TARGET_DYNAMIC)

lib-static: $(LIST_LIB_STATIC)

$(LIST_LIB_STATIC): $(LIST_OBJ)
	ar rc $@ $^

$(TARGET_STATIC): $(MAIN_OBJ) $(LIST_LIB_STATIC)
	$(CC) $(CFLAGS) $^ -o $@

lib-dynamic: $(LIST_LIB_DYNAMIC)

$(LIST_OBJ_PIC): %.pic.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(LIST_LIB_DYNAMIC): $(LIST_OBJ_PIC)
	$(CC) -shared $^ -o $@

$(TARGET_DYNAMIC): $(MAIN_OBJ) | $(LIST_LIB_DYNAMIC)
	$(CC) $(CFLAGS) $^ $(LIBFLAGS_DYNAMIC) -o $@

test: lib-static $(TEST_TARGET)

$(TEST_TARGET): $(TESTS) $(MAIN_OBJ) $(LIST_LIB_STATIC)
	$(CC) $(CFLAGS) $(TESTSFLAGS) $^ $(LDFLAGS) -o $(TEST_TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run-static:
	./$(TARGET_STATIC)

run-dynamic:
	./$(TARGET_DYNAMIC)

test-run: test
	./$(TEST_TARGET) --verbose --color=always

clean:
	rm -f $(TARGET_STATIC) $(TARGET_DYNAMIC) $(TEST_TARGET) \
	      $(LIST_LIB_STATIC) $(LIST_LIB_DYNAMIC) \
	      $(LIST_OBJ) $(LIST_OBJ_PIC) $(MAIN_OBJ) $(MAIN_OBJ_PIC)