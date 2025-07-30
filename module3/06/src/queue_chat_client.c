#include "queue_chat.h"

bool running = true;
int client_id;

void read_messages() {
  msgbuf msg;
  while (running) {
    if (msgrcv(msqid, &msg, sizeof(msgbuf) - sizeof(long), client_id, 0) ==
        -1) {
      perror("msgrcv");
      continue;
    }
    printf("Получено сообщение от %d : %s \n", msg.sender, msg.mtext);
  }
}

void send_message(int reciever, const char* str) {
  msgbuf out_msg;
  out_msg.sender = client_id;
  out_msg.mtype = reciever;
  strncpy(out_msg.mtext, str, MAX_MSG_SIZE);
  printf("Отправлено сообщение для %d \n", reciever);
  if (msgsnd(msqid, &out_msg, sizeof(msgbuf) - sizeof(long), 0) == -1) {
    perror("msgsnd");
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Недостаточно аргументов. Попробуйте ./client < client_id >\n");
    exit(EXIT_FAILURE);
  }
  client_id = atoi(argv[1]);

  if (client_id < SERVER_ID) {
    printf("Недопустимое значение client_id\n");
    exit(EXIT_FAILURE);
  }

  key_t key = ftok("server", 1);
  if ((msqid = msgget(key, 0666)) == -1) {
    perror("msgget");
    exit(EXIT_FAILURE);
  }

  printf("Клиент с id = %d запущен.\n", client_id);
  send_message(SERVER_ID, "connect");
  char input[MAX_MSG_SIZE];
  pid_t pid = fork();

  if (pid == 0) {  // Поток чтения
    read_messages();
    exit(EXIT_SUCCESS);
  }

  while (running) {
    printf("Введите сообщение (shutdown для выхода) >> ");
    fflush(stdout);
    fgets(input, MAX_MSG_SIZE, stdin);
    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "shutdown") == 0) {
      send_message(SERVER_ID, "shutdown");
      running = false;
    } else
      send_message(SERVER_ID, input);
  }
  kill(pid, SIGTERM);
  if (waitpid(pid, NULL, 0) == -1) {
    perror("waitpid");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}