#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define BUFFER_SIZE 1024
#define FILENAME_LENGHT 128
#define h_addr h_addr_list[0]

void error(const char *msg) {
  perror(msg);
  exit(0);
}

void send_file(int sockfd) {
  char buffer[BUFFER_SIZE] = {0};
  char filename[FILENAME_LENGHT];
  int file_fd;
  ssize_t bytes_read, bytes_sent;
  struct stat file_stat;
  printf("Введите имя файла: ");
  fgets(filename, FILENAME_LENGHT, stdin);
  filename[strcspn(filename, "\n")] = '\0';

  file_fd = open(filename, O_RDONLY);
  if (file_fd == -1) {
    perror("Ошибка при открытии файла");
    return;
  }

  if (fstat(file_fd, &file_stat) < 0) {
    perror("fstat");
    close(file_fd);
    return;
  }

  size_t filename_len = strlen(filename);
  send(sockfd, &filename_len, sizeof(filename_len), 0);
  send(sockfd, filename, filename_len, 0);
  send(sockfd, &file_stat.st_size, sizeof(&file_stat.st_size), 0);

  off_t total_sent = 0;

  while (total_sent < file_stat.st_size) {
    bytes_read = read(file_fd, buffer, BUFFER_SIZE);
    if (bytes_read <= 0) {
      break;
    }
    bytes_sent = send(sockfd, buffer, bytes_read, 0);
    if (bytes_sent <= 0) {
      perror("Ошибка при отправке файла");
      break;
    }

    total_sent += bytes_sent;
  }

  printf("Файл %s (%ld байт) успешно отправлен\n", filename,
         (long)file_stat.st_size);
}

int main(int argc, char *argv[]) {
  int my_sock, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char buff[BUFFER_SIZE];
  printf("TCP DEMO CLIENT\n");
  if (argc < 3) {
    fprintf(stderr, "usage %s hostname port\n", argv[0]);
    exit(0);
  }
  portno = atoi(argv[2]);
  my_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (my_sock < 0) error("ERROR opening socket");
  server = gethostbyname(argv[1]);
  if (server == NULL) {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(portno);

  if (connect(my_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR connecting");
  while ((n = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) > 0) {
    buff[n] = 0;
    printf("S=>C:%s", buff);
    printf("S<=C:");
    fgets(&buff[0], sizeof(buff) - 1, stdin);
    if (!strcmp(buff, "exit\n")) {
      send(my_sock, &buff[0], strlen(&buff[0]), 0);
      printf("Exit...");
      close(my_sock);
      return 0;
    } else if (!strcmp(buff, "file\n")) {
      send(my_sock, &buff[0], strlen(&buff[0]),
           0);  // Отправляем file cерверу чтобы он перешел к получению файла
      send_file(my_sock);
      continue;
    }
    send(my_sock, &buff[0], strlen(&buff[0]), 0);
  }

  if (n == 0) {
    printf("Server closed connection\n");
    close(my_sock);
    return 0;
  }
  printf("Recv error \n");
  close(my_sock);
  return -1;
}
