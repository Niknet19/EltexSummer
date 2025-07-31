#include <shmem.h>

// void mutex_lock(int semid) { semop(semid, &lock, 1); }
// void mutex_unlock(int semid) { semop(semid, unlock, 2); }

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
    sem_wait(mutex);
    generate_data(data);
    sem_post(mutex);

    while (1) {  // ждем ответа
      sem_wait(mutex);
      if (data->count == 0) {
        printf("Max: %d, Min: %d \n", data->max, data->min);
        sem_post(mutex);
        processed_nums++;
        break;
      }
      sem_post(mutex);
      usleep(1000);
    }
    sleep(1);
  }
}

void consume(SharedData *data) {
  while (1) {
    sem_wait(mutex);
    if (data->done) {
      sem_post(mutex);
      break;
    }
    process_data(data);
    sem_post(mutex);
    usleep(1000);
  }
  munmap(data, SHM_SIZE);  // Отключаем сегмент в этом процессе
}

int main() {
  srand(time(NULL));
  mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 1);
  if (mutex == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }
  shmid = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
  if (shmid == -1) {
    perror("shm_open");
    exit(1);
  }

  printf("Shmid : %d\n", shmid);

  if (ftruncate(shmid, SHM_SIZE) == -1) {
    perror("ftruncate");
  }

  //   if (semctl(semid, 0, SETVAL, 1) == -1) {
  //     perror("semctl");
  //   }

  signal(SIGINT, handle_signal);

  SharedData *data = (SharedData *)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE,
                                        MAP_SHARED, shmid, 0);
  if (data == (void *)-1) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
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

  sem_wait(mutex);
  data->done = 1;
  sem_post(mutex);
  wait(NULL);

  printf("\nОбработано %d строк\n", processed_nums);
  if (munmap(data, SHM_SIZE) == -1) {
    perror("munmap");
  }
  if (shm_unlink(SHM_NAME) == -1) {
    perror("shm_unlink");
  }
  if (sem_close(mutex) == -1) {
    perror("semctl");
  }

  if (sem_unlink(SEM_NAME) == -1) {
    perror("semctl");
  }

  return 0;
}