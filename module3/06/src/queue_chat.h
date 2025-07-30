#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

#define SERVER_ID 10
#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 256

int msqid;

typedef struct msgbuf {
  long mtype;               /* тип сообщения, должен быть > 0 */
  int sender;               // отправитель
  char mtext[MAX_MSG_SIZE]; /* содержание сообщения */
} msgbuf;