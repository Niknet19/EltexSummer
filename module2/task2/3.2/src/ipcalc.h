#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

u_int32_t parse_ip(const char* ip_as_str);

int ip_check(u_int32_t network_ip, u_int32_t mask, u_int32_t ip_to_check);

u_int32_t generate_random_ip();
