#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "checksum.h"

#define PCKT_LEN 1432

int sock;
uint32_t server_ip;
uint16_t server_port;
uint32_t client_ip;
uint16_t client_port;

void send_message(char *msg, int msg_len) {
  char buffer[PCKT_LEN];
  memset(buffer, 0, PCKT_LEN);

  struct iphdr *iph = (struct iphdr *)buffer;
  iph->version = 4;
  iph->ihl = 5;
  iph->tos = 0;
  iph->id = 0;
  iph->frag_off = 0;
  iph->ttl = 64;
  iph->protocol = IPPROTO_UDP;
  iph->saddr = client_ip;
  iph->daddr = server_ip;

  struct udphdr *udph = (struct udphdr *)(buffer + sizeof(struct iphdr));
  udph->source = htons(client_port);
  udph->dest = htons(server_port);
  udph->check = 0;

  char *payload = buffer + sizeof(struct iphdr) + sizeof(struct udphdr);
  memcpy(payload, msg, msg_len);
  int payload_len = msg_len;

  udph->len = htons(sizeof(struct udphdr) + payload_len);
  iph->tot_len =
      htons(sizeof(struct iphdr) + sizeof(struct udphdr) + payload_len);

  iph->check = 0;
  // iph->check = 0xffff;
  //  iph->check = csum((unsigned short *)buffer, iph->ihl * 4);

  struct sockaddr_in dest;
  dest.sin_family = AF_INET;
  dest.sin_addr.s_addr = server_ip;
  dest.sin_port = server_port;
  if (sendto(sock, buffer, ntohs(iph->tot_len), 0, (struct sockaddr *)&dest,
             sizeof(dest)) < 0) {
    perror("sendto failed");
  }
}

void signal_handler(int sig) {
  if (sig == SIGINT) {
    send_message("CLOSE", 5);
    close(sock);
    exit(0);
  }
}

void recieve_message(char *buffer) {
  while (1) {
    memset(buffer, 0, PCKT_LEN);
    int len = recv(sock, buffer, PCKT_LEN, 0);
    if (len < 0) {
      perror("recv");
      continue;
    }
    struct iphdr *iph = (struct iphdr *)buffer;
    if (iph->protocol != IPPROTO_UDP) continue;

    struct udphdr *udph = (struct udphdr *)(buffer + iph->ihl * 4);
    if (ntohs(udph->dest) != client_port) continue;

    char *data = buffer + iph->ihl * 4 + sizeof(struct udphdr);
    int data_len = ntohs(udph->len) - sizeof(struct udphdr);
    data[data_len] = '\0';
    printf("Server: %s\n", data);
    break;
  }
}
int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("Использование: %s <server_ip> <server_port> <client_port>\n",
           argv[0]);
    exit(1);
  }

  struct in_addr server_ip_addr, client_ip_addr;
  if (inet_pton(AF_INET, argv[1], &server_ip_addr) <= 0) {
    perror("Invalid server IP");
    exit(1);
  }
  server_ip = server_ip_addr.s_addr;
  server_port = atoi(argv[2]);
  client_port = atoi(argv[3]);

  inet_pton(AF_INET, "127.0.0.20", &client_ip_addr);
  client_ip = client_ip_addr.s_addr;

  sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }

  int one = 1;
  if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
    perror("setsockopt");
    exit(1);
  }

  signal(SIGINT, signal_handler);

  char input[1024];
  char buffer[PCKT_LEN];
  printf("Введите сообщение (Ctrl+C для выхода):\n");
  while (1) {
    if (fgets(input, sizeof(input), stdin) == NULL) break;
    input[strcspn(input, "\n")] = 0;
    int msg_len = strlen(input);
    if (msg_len == 0) continue;
    send_message(input, msg_len);
    recieve_message(buffer);
  }

  close(sock);
  return 0;
}