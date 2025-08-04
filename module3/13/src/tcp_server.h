// server.h
#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define OUTPUT_DIR "received_files"
#define BUFFER_SIZE 1024
#define FILENAME_LENGTH 128
#define MAX_CLIENTS 100
#define SHM_NAME "/nclients"

#define str1 "Enter 1 parameter\r\n"
#define str2 "Enter 2 parameter\r\n"
#define str3                                                               \
  "\033[1;32m Поддерживаемые операции: +-/* file для передачи файлов или " \
  "exit для выхода\033[0m\n"

typedef struct func {
  char symbol;
  int (*ptr)(int, int);
} func;

extern int *nclients;
extern func ops[];

void error(const char *msg);
void printusers(void);
void cleanup(int sig);
void handle_client(int sock);
void recv_file(int sockfd);
void setup_server_socket(int argc, char *argv[]);
void run_server(void);

#endif