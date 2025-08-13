#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "checksum.h"

#define PCKT_LEN 1432
#define MAX_CLIENTS 100

typedef struct client {
  uint32_t ip;
  uint16_t port;
  int count;
  bool active;
} client;

client clients[MAX_CLIENTS];

void init_clients() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    clients[i].active = 0;
  }
}

client *find_or_add_client(uint32_t ip, uint16_t port) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].active && clients[i].ip == ip && clients[i].port == port) {
      return &clients[i];
    }
  }
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (!clients[i].active) {
      clients[i].ip = ip;
      clients[i].port = port;
      clients[i].count = 0;
      clients[i].active = 1;
      char ip_str[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &ip, ip_str, INET6_ADDRSTRLEN);
      printf("Добавлен клиент: %s:%d\n", ip_str, port);
      return &clients[i];
    }
  }
  return NULL;
}

void remove_client(uint32_t ip, uint16_t port) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].active && clients[i].ip == ip && clients[i].port == port) {
      clients[i].active = 0;
      return;
    }
  }
}

void send_response(int sock, uint32_t src_ip, uint16_t src_port,
                   uint32_t dst_ip, uint16_t dst_port, char *data, int data_len,
                   int count) {
  char buffer[PCKT_LEN];
  memset(buffer, 0, PCKT_LEN);

  struct iphdr *iph = (struct iphdr *)buffer;
  iph->ihl = 5;
  iph->version = 4;
  iph->tos = 0;
  iph->id = 0;
  iph->frag_off = 0;
  iph->ttl = 64;
  iph->protocol = IPPROTO_UDP;
  iph->saddr = src_ip;
  iph->daddr = dst_ip;

  struct udphdr *udph = (struct udphdr *)(buffer + sizeof(struct iphdr));
  udph->source = htons(src_port);
  udph->dest = htons(dst_port);

  // Добавлюяем свои данные
  char *payload = buffer + sizeof(struct iphdr) + sizeof(struct udphdr);
  memcpy(payload, data, data_len);
  payload[data_len] = ' ';
  char num_str[10];
  sprintf(num_str, "%d", count);
  memcpy(payload + data_len + 1, num_str, strlen(num_str));
  int payload_len = data_len + 1 + strlen(num_str);

  udph->len = htons(sizeof(struct udphdr) + payload_len);
  iph->tot_len =
      htons(sizeof(struct iphdr) + sizeof(struct udphdr) + payload_len);

  iph->check = 0;
  // iph->check = csum((unsigned short *)buffer, iph->tot_len);

  udph->check = 0;

  struct sockaddr_in dest;
  dest.sin_family = AF_INET;
  dest.sin_addr.s_addr = dst_ip;
  if (sendto(sock, buffer, ntohs(iph->tot_len), 0, (struct sockaddr *)&dest,
             sizeof(dest)) < 0) {
    perror("sendto");
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <server_port>\n", argv[0]);
    exit(1);
  }
  uint16_t server_port = atoi(argv[1]);

  int sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }

  int one = 1;
  if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
    perror("setsockopt");
    exit(1);
  }

  init_clients();

  printf("Сервер запущен на порту %d ...\n", server_port);

  struct in_addr server_ip_addr;
  inet_pton(AF_INET, "127.0.0.1", &server_ip_addr);
  uint32_t server_ip = server_ip_addr.s_addr;

  char buffer[PCKT_LEN];

  while (1) {
    memset(buffer, 0, PCKT_LEN);
    struct sockaddr_in src_addr;
    socklen_t src_len = sizeof(src_addr);
    int len = recvfrom(sock, buffer, PCKT_LEN, 0, (struct sockaddr *)&src_addr,
                       &src_len);
    if (len < 0) {
      perror("recvfrom");
      continue;
    }

    struct iphdr *iph = (struct iphdr *)buffer;
    if (iph->protocol != IPPROTO_UDP) continue;

    struct udphdr *udph = (struct udphdr *)(buffer + iph->ihl * 4);
    uint16_t dst_port = ntohs(udph->dest);
    if (dst_port != server_port) continue;

    int udp_len = ntohs(udph->len);
    char *data = buffer + iph->ihl * 4 + sizeof(struct udphdr);
    int data_len = udp_len - sizeof(struct udphdr);

    uint32_t client_ip = iph->saddr;
    uint16_t client_port = ntohs(udph->source);

    if (strncmp(data, "CLOSE", 5) == 0 && data_len == 5) {
      remove_client(client_ip, client_port);
      continue;
    }

    struct client *cl = find_or_add_client(client_ip, client_port);
    cl->count++;
    int count = cl->count;

    send_response(sock, server_ip, server_port, client_ip, client_port, data,
                  data_len, count);
  }

  close(sock);
  return 0;
}