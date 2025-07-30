#include "queue_chat.h"

int active_clients[MAX_CLIENTS] = {0};

void cleanup() {
  if (msgctl(msqid, IPC_RMID, NULL) == -1) {
    perror("msgctl");
    exit(EXIT_FAILURE);
  } else {
    printf("Сервер отключен. Отчередь удалена.\n");
    exit(EXIT_SUCCESS);
  }
}

void handle_signal(int signal) {
  if (signal == SIGINT) {
    cleanup();
  }
}

bool add_client(int cliend_id) {
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    if (active_clients[i] == 0) {
      active_clients[i] = cliend_id;
      printf("Клиент %d подключился.\n", cliend_id);
      return true;
    }
  }
  printf("Превышел лимит подключений\n");
  return false;
}

bool remove_client(int cliend_id) {
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    if (active_clients[i] == cliend_id) {
      active_clients[i] = 0;
      printf("Клиент %d отключился.\n", cliend_id);
      return true;
    }
  }
  return false;
}

void broadcast_message(msgbuf* msg) {
  msgbuf out_msg;
  out_msg.sender = msg->sender;
  strncpy(out_msg.mtext, msg->mtext, MAX_MSG_SIZE);

  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    if (active_clients[i] != 0 && active_clients[i] != msg->sender) {
      out_msg.mtype = active_clients[i];
      if (msgsnd(msqid, &out_msg, sizeof(msgbuf) - sizeof(long), 0) == -1) {
        perror("msgsnd");
      }
      printf("Отправлено сообщение для %ld \n", out_msg.mtype);
      fflush(stdout);
    }
  }
}

int main() {
  key_t key = ftok("server", 1);
  if ((msqid = msgget(key, IPC_CREAT | 0666)) == -1) {
    perror("msgget");
    exit(EXIT_FAILURE);
  }
  signal(SIGINT, handle_signal);
  printf("Сервер запущен. Создана очередь с ID: %d\n", msqid);
  msgbuf msg;
  while (1) {
    if (msgrcv(msqid, &msg, sizeof(msgbuf) - sizeof(long), SERVER_ID, 0) ==
        -1) {
      perror("msgrsv");
      continue;
    }
    printf("Получено сообщение от %d \n", msg.sender);

    if (msg.sender != SERVER_ID) {
      if (strcmp(msg.mtext, "connect") == 0) {
        add_client(msg.sender);
      } else if (strcmp(msg.mtext, "shutdown") == 0) {
        remove_client(msg.sender);
      } else {
        broadcast_message(&msg);
      }
    }
  }
  return EXIT_SUCCESS;
}