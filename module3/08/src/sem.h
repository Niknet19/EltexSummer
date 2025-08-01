#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define FILE_NAME "shared_data.txt"
#define MAX_NUMBERS 20
#define MAX_LINES 10
#define MAX_PIDS 20
#define BUF_SIZE 256

typedef struct {
  int numbers[MAX_NUMBERS];
  int count;
} DataLine;

int mutex;
int empty;

struct sembuf lock = {0, -1, 0};
struct sembuf unlock[2] = {{0, 0, 0}, {0, 1, 0}};
struct sembuf unlock_empty = {0, 1, 0};
struct sembuf lock_empty = {0, -1, 0};
