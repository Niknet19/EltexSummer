#include "sem.h"

void cleanup(int sig) {
  if (sig == SIGINT) {
    semctl(mutex, 0, IPC_RMID);
    semctl(empty, 0, IPC_RMID);
    exit(EXIT_SUCCESS);
  }
}

void generate_data(DataLine *data) {
  data->count = rand() % MAX_NUMBERS + 1;
  for (int i = 0; i < data->count; i++) {
    data->numbers[i] = rand() % 100;
  }
}

int write_data_line(FILE *file, const DataLine *line) {
  if (file == NULL || line == NULL || line->count <= 0 ||
      line->count > MAX_NUMBERS) {
    return -1;
  }
  semop(mutex, &lock, 1);
  for (int i = 0; i < line->count; i++) {
    fprintf(file, "%d", line->numbers[i]);
    if (i < line->count - 1) {
      fprintf(file, " ");
    }
  }
  fprintf(file, "\n");
  fflush(file);
  semop(mutex, unlock, 2);
  return 0;
}

int read_data_line(FILE *file, DataLine *line) {
  if (file == NULL || line == NULL) {
    return -1;
  }

  semop(mutex, &lock, 1);

  if (feof(file)) {
    semop(mutex, unlock, 2);
    return -1;
  }

  char buffer[BUF_SIZE];
  if (fgets(buffer, sizeof(buffer), file) == NULL) {
    semop(mutex, unlock, 2);
    return -1;
  }

  line->count = 0;
  char *ptr = buffer;

  while (line->count < MAX_NUMBERS && *ptr != '\0' && *ptr != '\n') {
    int num;
    if (sscanf(ptr, "%d", &num) != 1) {
      break;
    }
    line->numbers[line->count++] = num;
    while (*ptr != ' ' && *ptr != '\n' && *ptr != '\0') ptr++;
    while (*ptr == ' ') ptr++;
  }

  semop(mutex, unlock, 2);
  return 0;
}

void process_data(DataLine *data, int *counter) {
  int max = data->numbers[0];
  int min = data->numbers[0];
  for (int i = 0; i < data->count; i++) {
    if (data->numbers[i] > max) max = data->numbers[i];
    if (data->numbers[i] < min) min = data->numbers[i];
  }
  (*counter)++;
  printf("%d PID: %d | Max: %d Min: %d\n", *counter, getpid(), max, min);
  fflush(stdout);
}

void producer() {
  FILE *file = fopen(FILE_NAME, "a");
  if (file == NULL) {
    perror("fopen in producer");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < MAX_LINES; i++) {
    DataLine data;
    generate_data(&data);
    if (write_data_line(file, &data) == 0) {
      semop(empty, &unlock_empty, 1);
    }
    sleep(1);
  }
  semop(empty, &unlock_empty, 1);
  fclose(file);
}

void consumer() {
  FILE *file = fopen(FILE_NAME, "r");
  if (file == NULL) {
    perror("fopen in consumer");
    exit(EXIT_FAILURE);
  }
  int counter = 0;
  DataLine data;
  while (1) {
    semop(empty, &lock_empty, 1);
    if (read_data_line(file, &data) == 0) {
      process_data(&data, &counter);
    } else {
      break;
    }
  }
  fclose(file);
}

int main(int argc, char **argv) {
  srand(time(NULL));
  size_t pids_number = 1;
  if (argc >= 2) pids_number = atoi(argv[1]);
  if (pids_number > MAX_PIDS) {
    printf("Слишком много процессов\n");
    exit(EXIT_FAILURE);
  }

  signal(SIGINT, cleanup);

  mutex = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
  if (mutex == -1) {
    perror("semget mutex");
    exit(EXIT_FAILURE);
  }
  if (semctl(mutex, 0, SETVAL, 1) == -1) {
    perror("semctl mutex");
    exit(EXIT_FAILURE);
  }

  empty = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
  if (empty == -1) {
    perror("semget empty");
    exit(EXIT_FAILURE);
  }
  if (semctl(empty, 0, SETVAL, 0) == -1) {
    perror("semctl empty");
    exit(EXIT_FAILURE);
  }

  unlock_empty.sem_op = pids_number;

  FILE *file = fopen(FILE_NAME, "w");
  if (file) fclose(file);

  pid_t pids[MAX_PIDS];
  for (size_t i = 0; i < pids_number; i++) {
    pids[i] = fork();
    if (pids[i] == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (pids[i] == 0) {
      consumer();
      exit(EXIT_SUCCESS);
    }
  }

  producer();

  for (size_t i = 0; i < pids_number; i++) {
    waitpid(pids[i], NULL, 0);
  }

  semctl(mutex, 0, IPC_RMID);
  semctl(empty, 0, IPC_RMID);
  return 0;
}