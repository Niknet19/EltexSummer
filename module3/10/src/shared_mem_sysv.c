#include <shmem.h>

void mutex_lock(int semid) { semop(semid, &lock, 1); }
void mutex_unlock(int semid) { semop(semid, unlock, 2); }

void handle_signal(int sig) {
  if (sig == SIGINT) is_running = 0;
  signal(SIGINT, SIG_DFL);
}

void generate_data(SharedData *data) {
  int count = rand() % MAX_NUMBERS + 1;
  for (int i = 0; i < count; i++) {
    data->numbers[i] = rand() % 1000;
  }
  data->count = count;
}

void process_data(SharedData *data) {
  if (data->count > 0) {
    int max = data->numbers[0];
    int min = data->numbers[0];
    for (int i = 0; i < data->count; i++) {
      if (data->numbers[i] > max) max = data->numbers[i];
      if (data->numbers[i] < min) min = data->numbers[i];
    }
    data->max = max;
    data->min = min;
    data->count = 0;
  }
}

void produce(SharedData *data) {
  while (is_running) {
    mutex_lock(semid);
    generate_data(data);
    mutex_unlock(semid);

    while (1) {  // ждем ответа
      mutex_lock(semid);
      if (data->count == 0) {
        printf("Max: %d, Min: %d \n", data->max, data->min);
        mutex_unlock(semid);
        processed_nums++;
        break;
      }
      mutex_unlock(semid);
      usleep(1000);
    }
    sleep(1);
  }
}

void consume(SharedData *data) {
  while (1) {
    mutex_lock(semid);
    if (data->done) {
      mutex_unlock(semid);
      break;
    }
    process_data(data);
    mutex_unlock(semid);
    usleep(1000);
  }
  shmdt(data);  // Отключаем сегмент в этом процессе
}

int main() {
  srand(time(NULL));
  semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
  if (semid == -1) {
    perror("semget");
    exit(EXIT_FAILURE);
  }
  shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
  if (shmid == -1) {
    perror("shmget");
    exit(1);
  }

  printf("Shmid : %d\n", shmid);

  if (semctl(semid, 0, SETVAL, 1) == -1) {
    perror("semctl");
  }

  signal(SIGINT, handle_signal);

  SharedData *data = (SharedData *)shmat(shmid, NULL, 0);
  data->done = 0;

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  }
  if (pid == 0) {
    consume(data);
  }
  produce(data);

  mutex_lock(semid);
  data->done = 1;
  mutex_unlock(semid);
  wait(NULL);

  printf("\nОбработано %d строк\n", processed_nums);
  shmdt(data);

  shmctl(shmid, IPC_RMID, NULL);
  semctl(semid, 0, IPC_RMID);
  return 0;
}