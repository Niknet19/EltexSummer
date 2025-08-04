#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_PACKET_SIZE 65535
#define DUMP_FILE "packet_dump.bin"

static int sockfd = -1;
static FILE *dump_file = NULL;

void signal_handler(int signum) {
  if (signum == SIGINT) {
    printf("Shutting down...\n");
    if (dump_file) {
      fclose(dump_file);
    }
    if (sockfd >= 0) {
      close(sockfd);
    }
    exit(EXIT_SUCCESS);
  }
}

void process_packet(unsigned char *buffer, int size, FILE *dump_file,
                    int target_port) {
  struct iphdr *iph = (struct iphdr *)buffer;
  if (iph->protocol == IPPROTO_UDP) {
    struct udphdr *udph = (struct udphdr *)(buffer + (iph->ihl * 4));
    int dest_port = ntohs(udph->dest);
    int src_port = ntohs(udph->source);

    if (dest_port == target_port) {
      unsigned char *data = buffer + (iph->ihl * 4) + sizeof(struct udphdr);
      int data_len = size - (iph->ihl * 4) - sizeof(struct udphdr);
      char src_ip[INET_ADDRSTRLEN];
      char dest_ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &iph->saddr, src_ip, INET_ADDRSTRLEN);
      inet_ntop(AF_INET, &iph->daddr, dest_ip, INET_ADDRSTRLEN);
      time_t now = time(NULL);
      printf(
          "[%s] Пакет src ip: %s  dest ip: %s src port: %d, dest port: %d "
          "размер: %d "
          "байт | "
          "Данные: %s\n",
          ctime(&now), src_ip, dest_ip, src_port, dest_port, data_len, data);
      fwrite(buffer, 1, size, dump_file);
      fflush(dump_file);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Использование: %s <target_port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int target_port = atoi(argv[1]);
  if (target_port <= 0 || target_port > 65535) {
    fprintf(stderr, "Invalid port number\n");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGINT, signal_handler) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }

  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  dump_file = fopen(DUMP_FILE, "wb");
  if (!dump_file) {
    perror("fopen");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  printf("Сниффер запущен %d...\n", target_port);
  printf("Пакеты сохраняются в файл %s\n", DUMP_FILE);

  unsigned char buffer[MAX_PACKET_SIZE];
  while (1) {
    int packet_size = recvfrom(sockfd, buffer, MAX_PACKET_SIZE, 0, NULL, NULL);
    if (packet_size < 0) {
      perror("recvfrom");
      continue;
    }
    process_packet(buffer, packet_size, dump_file, target_port);
  }

  fclose(dump_file);
  close(sockfd);
  return 0;
}