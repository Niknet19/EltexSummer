// server.c
#include "tcp_server.h"

int *nclients;
int sockfd = -1;

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int divide(int a, int b) { return a / b; }

func ops[] = {
    {'+', add},
    {'-', sub},
    {'*', mul},
    {'/', divide},
};

void error(const char *msg) {
  perror(msg);
  if (*nclients > 0) (*nclients)--;
  if (sockfd != -1) close(sockfd);
  shm_unlink(SHM_NAME);
  exit(EXIT_FAILURE);
}

void printusers(void) {
  printf(*nclients ? "%d user(s) on-line\n" : "No users online\n", *nclients);
}

void cleanup(int sig) {
  if (sig == SIGINT) {
    printf("\nShutting down server...\n");
    if (sockfd != -1) close(sockfd);
    shm_unlink(SHM_NAME);
    exit(EXIT_SUCCESS);
  }
}

void handle_client(int sock) {
  char buff[20 * 1024] = {0};
  int bytes_recv;
  char op;
  int a, b;

  while (1) {
    if (write(sock, str3, strlen(str3)) < 0) {
      perror("write failed");
      break;
    }
    bytes_recv = read(sock, buff, sizeof(buff) - 1);
    if (bytes_recv <= 0) break;
    buff[bytes_recv] = '\0';

    if (strcmp(buff, "exit\n") == 0) break;
    if (strcmp(buff, "file\n") == 0) {
      recv_file(sock);
      continue;
    }

    op = buff[0];
    printf("Operation: %c\n", op);

    if (write(sock, str1, strlen(str1)) < 0) {
      perror("write failed");
      break;
    }
    bytes_recv = read(sock, buff, sizeof(buff) - 1);
    if (bytes_recv <= 0) break;
    buff[bytes_recv] = '\0';
    a = atoi(buff);

    if (write(sock, str2, strlen(str2)) < 0) {
      perror("write failed");
      break;
    }
    bytes_recv = read(sock, buff, sizeof(buff) - 1);
    if (bytes_recv <= 0) break;
    buff[bytes_recv] = '\0';
    b = atoi(buff);

    for (size_t i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
      if (ops[i].symbol == op) {
        a = ops[i].ptr(a, b);
        printf("Result: %d\n", a);
        break;
      }
    }

    snprintf(buff, sizeof(buff), "%d\n", a);
    if (write(sock, buff, strlen(buff)) < 0) {
      perror("write failed");
      break;
    }
  }

  close(sock);
  (*nclients)--;
  printf("Client disconnected. ");
  printusers();
  munmap(nclients, sizeof(int));
}

void recv_file(int sockfd) {
  char buffer[BUFFER_SIZE] = {0};
  char filename[FILENAME_LENGTH] = {0};
  char filepath[FILENAME_LENGTH + sizeof(OUTPUT_DIR) + 1] = {0};
  int file_fd;
  ssize_t bytes_received;
  size_t filename_len;
  off_t file_size, total_received = 0;

  mkdir(OUTPUT_DIR, 0777);

  if (read(sockfd, &filename_len, sizeof(filename_len)) <= 0) {
    perror("Failed to receive filename length");
    return;
  }

  if (read(sockfd, filename, filename_len) <= 0) {
    perror("Failed to receive filename");
    return;
  }
  filename[filename_len] = '\0';

  if (read(sockfd, &file_size, sizeof(file_size)) <= 0) {
    perror("Failed to receive file size");
    return;
  }

  snprintf(filepath, sizeof(filepath), "%s/%s", OUTPUT_DIR, filename);

  file_fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (file_fd == -1) {
    perror("Failed to create file");
    return;
  }

  while (total_received < file_size) {
    bytes_received = read(sockfd, buffer, BUFFER_SIZE);
    if (bytes_received <= 0) break;

    if (write(file_fd, buffer, bytes_received) != bytes_received) {
      perror("Failed to write to file");
      break;
    }
    total_received += bytes_received;
  }

  close(file_fd);
  printf("File %s (%ld bytes) received successfully\n", filename,
         (long)file_size);
}

void setup_server_socket(int argc, char *argv[]) {
  struct sockaddr_in serv_addr;
  int portno;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  portno = atoi(argv[1]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR opening socket");

  int opt = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    error("setsockopt failed");
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    error("ERROR on binding");
  }

  if (listen(sockfd, MAX_CLIENTS) < 0) {
    error("ERROR on listen");
  }
}

void run_server(void) {
  struct sockaddr_in cli_addr;
  socklen_t clilen = sizeof(cli_addr);
  int newsockfd, pid;

  signal(SIGINT, cleanup);
  signal(SIGCHLD, SIG_IGN);  // ??

  printf("TCP SERVER DEMO\n");
  printusers();

  while (1) {
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0) {
      perror("ERROR on accept");
      continue;
    }

    (*nclients)++;
    struct hostent *hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
    printf("+%s [%s] new connection!\n", hst ? hst->h_name : "Unknown host",
           inet_ntoa(cli_addr.sin_addr));
    printusers();

    pid = fork();
    if (pid < 0) {
      perror("ERROR on fork");
      close(newsockfd);
      (*nclients)--;
      continue;
    }

    if (pid == 0) {
      close(sockfd);
      handle_client(newsockfd);
      exit(EXIT_SUCCESS);
    } else {
      close(newsockfd);
    }
  }
}

int main(int argc, char *argv[]) {
  int shmid = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (shmid == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }
  if (ftruncate(shmid, sizeof(int)) == -1) {
    perror("ftruncate");
  }
  nclients = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED,
                         shmid, 0);
  *nclients = 0;
  setup_server_socket(argc, argv);
  run_server();
  if (munmap(nclients, sizeof(int)) == -1) {
    perror("munmap");
  }
  if (shm_unlink(SHM_NAME) == -1) {
    perror("shm_unlink");
  }
  close(sockfd);
  return 0;
}