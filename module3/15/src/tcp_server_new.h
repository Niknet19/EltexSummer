#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100
#define SHM_NAME "/nclients"

#define str1 "Enter 1 parameter\r\n"
#define str2 "Enter 2 parameter\r\n"
#define str3 \
  "\033[1;32m Поддерживаемые операции: +-/* или exit для выхода\033[0m\n"

typedef struct {
  char symbol;
  int (*ptr)(int, int);
} func;

enum state {
  WAITING_FOR_OP,
  WAITING_FIRST_NUM,
  WAITING_SECOND_NUM,
} state;

typedef struct {
  int sockfd;
  int state;
  char op;
  int first_num;
  char buffer[BUFFER_SIZE];
} client_t;

int nclients;
int sockfd = -1;
client_t clients[MAX_CLIENTS];