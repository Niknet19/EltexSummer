#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_DRIVERS 100
#define BUFFER_SIZE 256
#define FIFO_DIR "/tmp/taxi_fifos/"

typedef struct {
  pid_t pid;
  int status;
  time_t task_end_time;
  int task_timer;
  char driver_fifo[BUFFER_SIZE];  // сервер -> водитель
  char server_fifo[BUFFER_SIZE];  // водитель -> сервер
} Driver;

Driver drivers[MAX_DRIVERS];
int driver_count = 0;

void create_fifo_dir() {
  struct stat st = {0};
  if (stat(FIFO_DIR, &st) == -1) {
    mkdir(FIFO_DIR, 0700);
  }
}

void driver_process(pid_t parent_pid) {
  signal(SIGPIPE, SIG_IGN);

  char driver_fifo[BUFFER_SIZE];
  char server_fifo[BUFFER_SIZE];

  snprintf(driver_fifo, BUFFER_SIZE, "%sdriver_%d", FIFO_DIR, getpid());
  snprintf(server_fifo, BUFFER_SIZE, "%sserver_%d", FIFO_DIR, getpid());

  if (mkfifo(driver_fifo, 0666) == -1 && errno != EEXIST) {
    perror("mkfifo driver");
  }

  if (mkfifo(server_fifo, 0666) == -1 && errno != EEXIST) {
    perror("mkfifo server");
    unlink(driver_fifo);
  }

  int driver_fd = open(driver_fifo, O_RDONLY);
  int server_fd = open(server_fifo, O_WRONLY);

  if (driver_fd == -1 || server_fd == -1) {
    perror("open fifo (child)");
    exit(EXIT_FAILURE);
  }

  fd_set readfds;
  char buffer[BUFFER_SIZE];
  time_t task_end = 0;
  int busy = 0;
  int remaining_time = 0;

  while (1) {
    FD_ZERO(&readfds);
    FD_SET(driver_fd, &readfds);

    int activity = select(driver_fd + 1, &readfds, NULL, NULL, NULL);

    if (activity < 0 && errno != EINTR) {
      perror("select");
      break;
    }

    // Кончилась ли задача
    if (busy && time(NULL) >= task_end) {
      busy = 0;
      remaining_time = 0;
      char status_msg[BUFFER_SIZE];
      snprintf(status_msg, BUFFER_SIZE, "PID %d STATUS Available\n", getpid());
      write(server_fd, status_msg, strlen(status_msg));
    }

    if (activity > 0 && FD_ISSET(driver_fd, &readfds)) {
      ssize_t bytes_read = read(driver_fd, buffer, BUFFER_SIZE - 1);
      if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("%s\n", buffer);
        fflush(stdout);

        if (strncmp(buffer, "TASK", 4) == 0) {
          int timer = atoi(buffer + 5);
          if (!busy) {
            busy = 1;
            task_end = time(NULL) + timer;
            remaining_time = timer;
            char status_msg[BUFFER_SIZE];
            snprintf(status_msg, BUFFER_SIZE, "PID %d STATUS Busy %d\n",
                     getpid(), remaining_time);
            write(server_fd, status_msg, strlen(status_msg));
          } else {
            int now_remaining = task_end - time(NULL);
            if (now_remaining < 0) now_remaining = 0;
            char error_msg[BUFFER_SIZE];
            snprintf(error_msg, BUFFER_SIZE, "PID %d ERROR Busy %d\n", getpid(),
                     now_remaining);
            write(server_fd, error_msg, strlen(error_msg));
          }
        } else if (strncmp(buffer, "STATUS", 6) == 0) {
          if (busy) {
            int now_remaining = task_end - time(NULL);
            if (now_remaining < 0) now_remaining = 0;
            char status_msg[BUFFER_SIZE];
            snprintf(status_msg, BUFFER_SIZE, "PID %d STATUS Busy %d\n",
                     getpid(), now_remaining);
            write(server_fd, status_msg, strlen(status_msg));
          } else {
            char status_msg[BUFFER_SIZE];
            snprintf(status_msg, BUFFER_SIZE, "PID %d STATUS Available\n",
                     getpid());
            write(server_fd, status_msg, strlen(status_msg));
          }
        }
      } else if (bytes_read == 0) {
        close(driver_fd);
        driver_fd = open(driver_fifo, O_RDONLY | O_NONBLOCK);
      }
    }
  }

  close(driver_fd);
  close(server_fd);
  unlink(driver_fifo);
  unlink(server_fifo);
  _exit(EXIT_SUCCESS);
}

void cleanup(int sig) {
  (void)sig;

  for (int i = 0; i < driver_count; i++) {
    if (drivers[i].pid > 0) {
      kill(drivers[i].pid, SIGTERM);
    }
  }

  for (int i = 0; i < driver_count; i++) {
    if (drivers[i].pid > 0 && kill(drivers[i].pid, 0) == 0) {
      kill(drivers[i].pid, SIGKILL);
    }
  }
  int status;
  pid_t pid;
  for (int i = 0; i < 3; i++) {
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    }
    sleep(1);
  }

  for (int i = 0; i < driver_count; i++) {
    if (drivers[i].driver_fifo[0] != '\0') {
      if (unlink(drivers[i].driver_fifo) == -1) {
        perror("unlink driver_fifo");
      }
    }
    if (drivers[i].server_fifo[0] != '\0') {
      if (unlink(drivers[i].server_fifo) == -1) {
        perror("unlink server_fifo");
      }
    }
  }

  if (sig != 0) {
    exit(EXIT_SUCCESS);
  }
}

int create_driver() {
  pid_t pid = fork();

  if (pid == 0) {
    driver_process(getppid());
  } else if (pid > 0) {
    Driver *d = &drivers[driver_count];
    d->pid = pid;
    d->status = 0;
    d->task_end_time = 0;
    d->task_timer = 0;
    snprintf(d->driver_fifo, BUFFER_SIZE, "%sdriver_%d", FIFO_DIR, pid);
    snprintf(d->server_fifo, BUFFER_SIZE, "%sserver_%d", FIFO_DIR, pid);

    driver_count++;
    printf("Водитель создан с PID: %d\n", pid);
    return 0;
  } else {
    perror("fork");
    return -1;
  }
}

void send_task(pid_t pid, int task_timer) {
  for (int i = 0; i < driver_count; i++) {
    if (drivers[i].pid == pid) {
      int fd = open(drivers[i].driver_fifo, O_WRONLY);
      if (fd == -1) {
        perror("open driver fifo");
        return;
      }
      char task_msg[BUFFER_SIZE];
      snprintf(task_msg, BUFFER_SIZE, "TASK %d\n", task_timer);
      write(fd, task_msg, strlen(task_msg));
      close(fd);
      printf("Задача отправлена водителю %d на %d секунд\n", pid, task_timer);

      int server_fd = open(drivers[i].server_fifo, O_RDONLY | O_NONBLOCK);
      if (server_fd != -1) {
        char buffer[BUFFER_SIZE];
        fd_set readfds;
        // struct timeval timeout;

        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);

        // timeout.tv_sec = 1;
        // timeout.tv_usec = 0;

        if (select(server_fd + 1, &readfds, NULL, NULL, NULL) > 0) {
          ssize_t bytes_read = read(server_fd, buffer, BUFFER_SIZE - 1);
          if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Ответ водителя %d: %s", pid, buffer);
          }
        }
        close(server_fd);
      }

      return;
    }
  }
  printf("Водитель с PID %d не найден\n", pid);
}

void get_status(pid_t pid) {
  for (int i = 0; i < driver_count; i++) {
    if (drivers[i].pid == pid) {
      int fd = open(drivers[i].driver_fifo, O_WRONLY | O_NONBLOCK);
      if (fd == -1) {
        perror("open driver fifo");
        return;
      }
      write(fd, "STATUS\n", 7);
      close(fd);

      int server_fd = open(drivers[i].server_fifo, O_RDONLY | O_NONBLOCK);
      if (server_fd == -1) {
        perror("open server fifo");
        return;
      }

      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(server_fd, &readfds);

      //   struct timeval timeout;
      //   timeout.tv_sec = 2;
      //   timeout.tv_usec = 0;

      if (select(server_fd + 1, &readfds, NULL, NULL, NULL) > 0) {
        if (FD_ISSET(server_fd, &readfds)) {
          char buffer[BUFFER_SIZE];
          ssize_t bytes_read = read(server_fd, buffer, BUFFER_SIZE - 1);
          if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Водитель %d: %s", pid, buffer);
          }
        }
      }
      close(server_fd);
      return;
    }
  }
  printf("Водитель с PID %d не найден\n", pid);
}

void get_drivers() {
  printf("Активные водители:\n");
  for (int i = 0; i < driver_count; i++) {
    get_status(drivers[i].pid);
  }
}

int main() {
  create_fifo_dir();
  signal(SIGINT, cleanup);
  signal(SIGTERM, cleanup);

  printf("Система управления такси\n");
  printf("Доступные команды:\n");
  printf("  create_driver - создать водителя\n");
  printf("  send_task <pid> <время_задачи> - отправить задачу\n");
  printf("  get_status <pid> - получить статус водителя\n");
  printf("  get_drivers - показать всех водителей\n");
  printf("  exit - выход\n\n");

  char input[BUFFER_SIZE];

  while (1) {
    printf("> ");
    fflush(stdout);

    if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
      break;
    }

    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "exit") == 0) {
      break;
    } else if (strcmp(input, "create_driver") == 0) {
      create_driver();
    } else if (strcmp(input, "get_drivers") == 0) {
      get_drivers();
    } else if (strncmp(input, "send_task ", 10) == 0) {
      pid_t pid;
      int task_timer;
      if (sscanf(input + 10, "%d %d", &pid, &task_timer) == 2) {
        send_task(pid, task_timer);
      } else {
        printf("Использование: send_task <pid> <время_задачи>\n");
      }
    } else if (strncmp(input, "get_status ", 11) == 0) {
      pid_t pid;
      if (sscanf(input + 11, "%d", &pid) == 1) {
        get_status(pid);
      } else {
        printf("Использование: get_status <pid>\n");
      }
    } else if (strlen(input) > 0) {
      printf("Неизвестная команда: %s\n", input);
    }
  }

  cleanup(0);
  return 0;
}