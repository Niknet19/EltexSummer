#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define FILE_NAME "shared_data.txt"
#define MAX_NUMBERS 20
#define MAX_LINES 30
#define SEM_NAME "/data_sem"
#define EMPTY_SEM "/empty_sem"

sem_t *sem, *empty;

typedef struct {
  int numbers[MAX_NUMBERS];
  int count;
} DataLine;