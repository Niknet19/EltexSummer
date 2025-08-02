#include "udp_client.h"

void* receive_messages() {
  char buffer[MAX_MESSAGE_SIZE];
  // time_t currentTime;
  while (1) {
    int rec_len = recvfrom(sockfd, buffer, MAX_MESSAGE_SIZE, 0,
                           (struct sockaddr*)&other_addr, &addr_len);
    if (rec_len > 0) {
      buffer[rec_len] = '\0';
      // time(&currentTime);
      //    printf("\n%s от %s:%d  | %s\n", ctime(&currentTime),
      //           inet_ntoa(other_addr.sin_addr), ntohs(other_addr.sin_port),
      //           buffer);

      printf("\nПолучено от %s:%d  | %s\n", inet_ntoa(other_addr.sin_addr),
             ntohs(other_addr.sin_port), buffer);
      printf("Введите сообщение: ");
      fflush(stdout);
    }
  }
  return NULL;
}

void send_messages() {
  char message[MAX_MESSAGE_SIZE];
  while (1) {
    printf("Введите сообщение: ");
    if (fgets(message, MAX_MESSAGE_SIZE, stdin) == NULL) {
      break;
    }
    message[strcspn(message, "\n")] = '\0';

    if (strcmp(message, "quit") == 0) {
      break;
    }

    if (sendto(sockfd, message, strlen(message), 0,
               (const struct sockaddr*)&other_addr, addr_len) < 0) {
      perror("sendto");
    }
  }
}

int main(int argc, char** argv) {
  if (argc != 4) {
    printf("Использование: %s <my_port> <other_ip> <other_port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int my_port = atoi(argv[1]);
  char* other_ip = argv[2];
  int other_port = atoi(argv[3]);

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // memset(&my_addr, 0, sizeof(my_addr));
  bzero(&my_addr, sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = INADDR_ANY;
  my_addr.sin_port = htons(my_port);

  // memset(&other_addr, 0, sizeof(other_addr));
  bzero(&other_addr, sizeof(other_addr));
  other_addr.sin_family = AF_INET;
  other_addr.sin_port = htons(other_port);

  if (inet_pton(AF_INET, other_ip, &other_addr.sin_addr) <= 0) {
    printf("Невалидный IP адрес\n");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  if (bind(sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0) {
    perror("bind");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  printf("Чат запущен на порту %d. Присоединение к %s:%d\n", my_port, other_ip,
         other_port);

  pthread_t recv_thread;
  if (pthread_create(&recv_thread, NULL, receive_messages, NULL) != 0) {
    perror("pthread_create");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  send_messages();

  pthread_cancel(recv_thread);
  pthread_join(recv_thread, NULL);
  close(sockfd);
  printf("Chat ended\n");
  return 0;
}