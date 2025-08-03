#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OUTPUT_DIR "received_files"
#define BUFFER_SIZE 1024
#define FILENAME_LENGHT 128

#define str1 "Enter 1 parameter\r\n"
#define str2 "Enter 2 parameter\r\n"
#define str3                                                               \
  "\033[1;32m Поддерживаемые операции: +-/* file для передачи файлов или " \
  "exit для "                                                              \
  "выхода\033[0m\n"

// функция обслуживания
// подключившихся пользователей
void dostuff(int);

void recv_file(int sockfd);

// количество активных пользователей
int nclients = 0;
int sockfd, newsockfd;
// функция обработки ошибок
void error(const char *msg) {
  perror(msg);
  if (nclients > 0) nclients--;
  close(sockfd);
  close(newsockfd);
  exit(1);
}

// печать количества активных
// пользователей
void printusers() {
  if (nclients) {
    printf("%d user on-line\n", nclients);
  } else {
    printf("No User on line\n");
  }
}

typedef struct func {
  char symbol;
  int (*ptr)(int, int);
} func;

int add(int a, int b) { return a + b; }
int sub(int a, int b) {
  printf("minus\n");
  return a - b;
}
int mul(int a, int b) { return a * b; }
int divide(int a, int b) { return a / b; }

func ops[] = {
    {'+', add},
    {'-', sub},
    {'*', mul},
    {'/', divide},
};

void cleanup(int sig) {
  if (sig == SIGINT) {
    close(sockfd);
    close(newsockfd);
    exit(EXIT_FAILURE);
  }
}

// функция обработки данных

int main(int argc, char *argv[]) {
  // char buff[1024];        // Буфер для различных нужд
  // дескрипторы сокетов
  int portno;        // номер порта
  int pid;           // id номер потока
  socklen_t clilen;  // размер адреса клиента типа socklen_t
  struct sockaddr_in serv_addr, cli_addr;  // структура сокета сервера и клиента
  signal(SIGINT, cleanup);
  printf("TCP SERVER DEMO\n");
  // ошибка в случае если мы не указали порт
  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  // Шаг 1 - создание сокета
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR opening socket");
  // Шаг 2 - связывание сокета с локальным адресом
  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr =
      INADDR_ANY;  // сервер принимает подключения на все
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");
  // Шаг 3 - ожидание подключений, размер очереди - 5
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  // Шаг 4 - извлекаем сообщение из очереди (цикл извлечения запросов на
  // подключение)
  while (1) {
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0) error("ERROR on accept");
    nclients++;
    // вывод сведений о клиенте
    struct hostent *hst;
    hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
    printf("+%s [%s] new connect!\n", (hst) ? hst->h_name : "Unknown host",
           (char *)inet_ntoa(cli_addr.sin_addr));
    printusers();
    pid = fork();
    if (pid < 0) error("ERROR on fork");
    if (pid == 0) {
      close(sockfd);
      dostuff(newsockfd);
      exit(0);
    } else
      close(newsockfd);
  }
  close(sockfd);
  return 0;
}

void dostuff(int sock) {
  int bytes_recv;  // размер принятого сообщения
  char op;
  int a, b;  // переменные для myfunc
  char buff[20 * 1024];

  while (1) {
    write(sock, str3, strlen(str3));
    bytes_recv = read(sock, &buff[0], sizeof(buff));
    if (bytes_recv < 0) error("ERROR reading from socket");
    buff[bytes_recv] = '\0';
    if (strcmp(buff, "exit\n") == 0) {
      break;
    }
    if (strcmp(buff, "file\n") == 0) {
      recv_file(sock);
      continue;
    }
    op = (char)buff[0];
    printf("%c\n", op);
    // отправляем клиенту сообщение
    write(sock, str1, sizeof(str1));
    // обработка первого параметра
    bytes_recv = read(sock, &buff[0], sizeof(buff));
    if (bytes_recv < 0) error("ERROR reading from socket");
    a = atoi(buff);  // преобразование первого параметра в int
    // отправляем клиенту сообщение
    write(sock, str2, sizeof(str2));
    bytes_recv = read(sock, &buff[0], sizeof(buff));
    if (bytes_recv < 0) error("ERROR reading from socket");
    b = atoi(buff);  // преобразование второго параметра в int
    // a = ops->symbol(a, b);  // вызов пользовательской функции
    for (size_t i = 0; i < 4; i++) {
      if (ops[i].symbol == op) {
        a = ops[i].ptr(a, b);
        printf("res : %d\n", a);
        break;
      }
    }
    memset(buff, 0, sizeof(buff));
    snprintf(buff, sizeof(buff), "%d\n",
             a);                // преобразование результата в строку
    buff[strlen(buff)] = '\n';  // добавление к сообщению символа конца строки
    // отправляем клиенту результат
    write(sock, buff, strlen(buff));
  }

  nclients--;  // уменьшаем счетчик активных клиентов
  printf("-disconnect\n");
  printusers();
  return;
}

void recv_file(int sockfd) {
  char buffer[BUFFER_SIZE] = {0};
  int file_fd;
  ssize_t bytes_received, bytes_written;
  size_t filename_len;
  char filename[FILENAME_LENGHT];
  off_t file_size;
  off_t total_received = 0;

  mkdir(OUTPUT_DIR, 0777);
  printf("Принимаю файл ... \n");
  recv(sockfd, &filename_len, sizeof(filename_len),
       0);                               // Получаем размер имени файла
  read(sockfd, filename, filename_len);  // Получаем имя файла
  printf("Имя файла: %s\n", filename);
  read(sockfd, &file_size, sizeof(file_size));
  printf("Размер файла: %ld\n", file_size);
  char filepath[512];
  snprintf(filepath, sizeof(filepath), "%s/%s", OUTPUT_DIR, filename);

  file_fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (file_fd == -1) {
    perror("Ошибка при создании файла");
    return;
  }

  while (total_received < file_size) {
    bytes_received = read(sockfd, buffer, BUFFER_SIZE);
    if (bytes_received <= 0) {
      break;
    }
    bytes_written = write(file_fd, buffer, bytes_received);
    if (bytes_written <= 0) {
      perror("Ошибка при записи в файл");
      break;
    }
    total_received += bytes_received;
  }
  printf("Файл %s (%ld байт) успешно получен и сохранен как %s\n", filename,
         (long)file_size, filepath);
}