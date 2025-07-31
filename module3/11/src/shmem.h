#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_NUMBERS 100
#define SHM_SIZE sizeof(SharedData)
#define SHM_NAME "/shared_mem"
typedef struct SharedData {
  int numbers[MAX_NUMBERS];
  int count;
  int max;
  int min;
  int done;
} SharedData;

int semid;
int shmid;

static int processed_nums = 0;

struct sembuf lock = {0, -1, 0};
struct sembuf unlock[2] = {{0, 0, 0}, {0, 1, 0}};

static sig_atomic_t is_running = 1;