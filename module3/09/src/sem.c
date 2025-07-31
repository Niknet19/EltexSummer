#include "sem.h"

void cleanup(int sig) {
  if (sig == SIGINT) {
    sem_close(sem);
    sem_unlink(SEM_NAME);
    sem_close(empty);
    sem_unlink(EMPTY_SEM);
    exit(EXIT_SUCCESS);
  }
}

void generate_data(DataLine *data) {
  data->count = rand() % MAX_NUMBERS + 1;
  for (int i = 0; i < data->count; i++) {
    data->numbers[i] = rand() % 100;
  }
}

int write_data_line(FILE *file, const DataLine *line, sem_t *sem) {
  if (file == NULL || line == NULL || line->count <= 0 ||
      line->count > MAX_NUMBERS) {
    return -1;
  }
  sem_wait(sem);
  for (int i = 0; i < line->count; i++) {
    fprintf(file, "%d", line->numbers[i]);
    if (i < line->count - 1) {
      fprintf(file, " ");
    }
  }
  fprintf(file, "\n");
  fflush(file);
  sem_post(sem);
  return 0;
}

int read_data_line(FILE *file, DataLine *line, sem_t *sem) {
  if (file == NULL || line == NULL) {
    return -1;
  }

  sem_wait(sem);

  if (feof(file)) {
    sem_post(sem);
    return -1;
  }

  char buffer[256];
  if (fgets(buffer, sizeof(buffer), file) == NULL) {
    if (feof(file)) {
      sem_post(sem);
      return -1;
    } else {
      sem_post(sem);
      return -1;
    }
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

  sem_post(sem);
  return 0;
}

void process_data(DataLine *data) {
  int max = data->numbers[0];
  int min = data->numbers[0];
  for (int i = 0; i < data->count; i++) {
    if (data->numbers[i] > max) max = data->numbers[i];
    if (data->numbers[i] < min) min = data->numbers[i];
  }
  printf("Max: %d Min: %d\n", max, min);
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
    write_data_line(file, &data, sem);
    sem_post(empty);
    sleep(1);
  }
  sem_post(empty);
  fclose(file);
}

void consumer() {
  FILE *file = fopen(FILE_NAME, "r");
  if (file == NULL) {
    perror("fopen in consumer");
    exit(EXIT_FAILURE);
  }

  DataLine data;
  int lines_read = 0;

  while (1) {
    sem_wait(empty);
    if (read_data_line(file, &data, sem) == 0) {
      process_data(&data);
      lines_read++;
    } else {
      break;
    }
  }
  fclose(file);
}

int main() {
  signal(SIGINT, cleanup);
  sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 1);
  if (sem == SEM_FAILED) {
    perror("sem_open sem");
    exit(EXIT_FAILURE);
  }

  empty = sem_open(EMPTY_SEM, O_CREAT | O_EXCL, 0644, 0);
  if (empty == SEM_FAILED) {
    perror("sem_open empty");
    exit(EXIT_FAILURE);
  }

  FILE *file = fopen(FILE_NAME, "w");
  if (file) fclose(file);

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    producer();
    exit(EXIT_SUCCESS);
  } else {
    consumer();
    wait(NULL);
  }

  sem_close(sem);
  sem_unlink(SEM_NAME);
  sem_close(empty);
  sem_unlink(EMPTY_SEM);

  return 0;
}