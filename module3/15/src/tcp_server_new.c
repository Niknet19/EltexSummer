#include "tcp_server_new.h"

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int divide(int a, int b) { return b != 0 ? a / b : 0; }

func ops[] = {
    {'+', add},
    {'-', sub},
    {'*', mul},
    {'/', divide},
};

void error(const char *msg) {
  perror(msg);
  if (nclients > 0) (nclients)--;
  if (sockfd != -1) close(sockfd);
  exit(EXIT_FAILURE);
}

void printusers(void) {
  printf(nclients ? "%d user(s) on-line\n" : "No users online\n", nclients);
}

void cleanup(int sig) {
  if (sig == SIGINT) {
    printf("\nShutting down server...\n");
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (clients[i].sockfd > 0) close(clients[i].sockfd);
    }
    if (sockfd != -1) close(sockfd);
    exit(EXIT_SUCCESS);
  }
}

void init_client(int index, int new_sockfd) {
  clients[index].sockfd = new_sockfd;
  clients[index].state = WAITING_FOR_OP;
  memset(clients[index].buffer, 0, BUFFER_SIZE);
  if (write(new_sockfd, str3, strlen(str3)) < 0) {
    perror("write failed");
    close(new_sockfd);
    clients[index].sockfd = 0;
  }
}

void close_client(int index) {
  close(clients[index].sockfd);
  clients[index].sockfd = 0;
  clients[index].state = WAITING_FOR_OP;
  (nclients)--;
  printf("Client disconnected. ");
  printusers();
}

void process_client_data(int index) {
  client_t *client = &clients[index];
  char *msg = client->buffer;
  int bytes_read = read(client->sockfd, msg, BUFFER_SIZE - 1);

  if (bytes_read <= 0) {
    close_client(index);
    return;
  }

  char *newline = strchr(msg, '\n');
  if (!newline) return;

  *newline = '\0';

  switch (client->state) {
    case WAITING_FOR_OP:
      if (strcmp(msg, "exit") == 0) {
        close_client(index);
        return;
      }
      if (strlen(msg) == 1 &&
          (msg[0] == '+' || msg[0] == '-' || msg[0] == '*' || msg[0] == '/')) {
        client->op = msg[0];
        client->state = WAITING_FIRST_NUM;
        if (write(client->sockfd, str1, strlen(str1)) < 0) {
          perror("write");
          close_client(index);
        }
      } else {
        if (write(client->sockfd, str3, strlen(str3)) < 0) {
          perror("write");
          close_client(index);
        }
      }
      break;

    case WAITING_FIRST_NUM:
      client->first_num = atoi(msg);
      client->state = WAITING_SECOND_NUM;
      if (write(client->sockfd, str2, strlen(str2)) < 0) {
        perror("write");
        close_client(index);
      }
      break;

    case WAITING_SECOND_NUM:
      int second_num = atoi(msg);
      int result = 0;
      for (size_t i = 0; i < sizeof(ops) / sizeof(ops[0]); i++) {
        if (ops[i].symbol == client->op) {
          result = ops[i].ptr(client->first_num, second_num);
          break;
        }
      }
      printf("Op: %c %d %d = %d\n", client->op, client->first_num, second_num,
             result);
      snprintf(msg, BUFFER_SIZE, "%d\n", result);
      if (write(client->sockfd, msg, strlen(msg)) < 0) {
        perror("write");
        close_client(index);
        return;
      }
      client->state = WAITING_FOR_OP;
      if (write(client->sockfd, str3, strlen(str3)) < 0) {
        perror("write");
        close_client(index);
      }
      break;
  }
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
    error("setsockopt");
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
  fd_set read_fds;
  struct sockaddr_in cli_addr;
  socklen_t clilen = sizeof(cli_addr);

  signal(SIGINT, cleanup);
  printf("TCP SERVER DEMO\n");
  printusers();

  while (1) {
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);
    int max_fd = sockfd;

    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (clients[i].sockfd > 0) {
        FD_SET(clients[i].sockfd, &read_fds);
        if (clients[i].sockfd > max_fd) max_fd = clients[i].sockfd;
      }
    }

    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
      error("select error");
    }

    if (FD_ISSET(sockfd, &read_fds)) {
      int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
      if (newsockfd < 0) {
        perror("ERROR on accept");
        continue;
      }

      int free_slot = -1;
      for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd == 0) {
          free_slot = i;
          break;
        }
      }

      if (free_slot == -1) {
        printf("Too many clients!\n");
        close(newsockfd);
      } else {
        (nclients)++;
        struct hostent *hst =
            gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
        printf("+%s [%s] : [%d] new connection!\n",
               hst ? hst->h_name : "Unknown host", inet_ntoa(cli_addr.sin_addr),
               cli_addr.sin_port);
        printusers();
        init_client(free_slot, newsockfd);
      }
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (clients[i].sockfd > 0 && FD_ISSET(clients[i].sockfd, &read_fds)) {
        process_client_data(i);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  nclients = 0;

  memset(clients, 0, sizeof(clients));

  setup_server_socket(argc, argv);
  run_server();
  close(sockfd);
  return 0;
}