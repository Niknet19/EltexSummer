
#include "ipcalc.h"

// Парсим строку в байтовое представление ip адресв
u_int32_t parse_ip(const char* ip_as_str) {
  u_int32_t ip_as_uint = 0;
  u_int8_t octets[4];
  char* token;
  int i = 0;

  // создаем изменяемую копию строки, с const char* strtok не работает
  char* ip_copy = strdup(ip_as_str);
  token = strtok(ip_copy, ".");
  while (token != NULL && i < 4) {
    octets[i] = (u_int8_t)atoi(token);
    token = strtok(NULL, ".");
    i++;
  }

  for (size_t i = 0; i < 4; i++) {
    ip_as_uint = ip_as_uint << 8;
    ip_as_uint |= octets[i];
  }
  return ip_as_uint;
}

// Проверка на принадлежность ip к подсети с маской mask
int ip_check(u_int32_t network_ip, u_int32_t mask, u_int32_t ip_to_check) {
  u_int32_t subnetwork_ip = network_ip & mask;
  u_int32_t subnetwork_checked_ip = ip_to_check & mask;
  return subnetwork_ip == subnetwork_checked_ip;
}

u_int32_t generate_random_ip() {
  return ((u_int32_t)rand() % 256 << 24) | ((u_int32_t)rand() % 256 << 16) |
         ((u_int32_t)rand() % 256 << 8) | ((u_int32_t)rand() % 256);
}

#ifdef CRITERION_TESTING

#else
int main(int argc, char* argv[]) {
  if (argc < 4) return 0;
  u_int32_t gateway_ip = parse_ip(argv[1]);
  u_int32_t subnet_musk_ip = parse_ip(argv[2]);
  int total_ips = atoi(argv[3]);
  int ips_in_subnet = 0;
  srand((u_int32_t)time(NULL));
  for (int i = 0; i < total_ips; i++) {
    ips_in_subnet += ip_check(gateway_ip, subnet_musk_ip, generate_random_ip());
  }
  printf("Принадлежат подсети: %d\n", ips_in_subnet);
  printf("Другим сетям: %d\n", total_ips - ips_in_subnet);
  printf("Всего: %d\n", total_ips);

  printf("Процент: %.3lf %%",
         ((double)ips_in_subnet / (double)total_ips) * 100.0);
  return 0;
}
#endif