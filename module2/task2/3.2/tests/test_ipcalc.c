#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../src/ipcalc.h"

Test(parse_ip, valid_ip) {
  u_int32_t ip = parse_ip("192.168.1.1");
  cr_assert_eq(ip, 0xC0A80101);
}

Test(parse_ip, another_valid_ip) {
  u_int32_t ip = parse_ip("10.0.0.1");
  cr_assert_eq(ip, 0x0A000001);
}

Test(parse_ip, max_values) {
  u_int32_t ip = parse_ip("255.255.255.255");
  cr_assert_eq(ip, 0xFFFFFFFF);
}

Test(parse_ip, min_values) {
  u_int32_t ip = parse_ip("0.0.0.0");
  cr_assert_eq(ip, 0x00000000);
}

Test(ip_check, same_subnet) {
  u_int32_t network_ip = parse_ip("192.168.1.1");
  u_int32_t mask = parse_ip("255.255.255.0");
  u_int32_t ip_to_check = parse_ip("192.168.1.100");

  cr_assert(ip_check(network_ip, mask, ip_to_check));
}

Test(ip_check, different_subnet) {
  u_int32_t network_ip = parse_ip("192.168.1.1");
  u_int32_t mask = parse_ip("255.255.255.0");
  u_int32_t ip_to_check = parse_ip("192.168.2.100");

  cr_assert_not(ip_check(network_ip, mask, ip_to_check));
}

Test(ip_check, different_mask) {
  u_int32_t network_ip = parse_ip("192.168.1.1");
  u_int32_t mask = parse_ip("255.255.0.0");
  u_int32_t ip_to_check = parse_ip("192.168.2.100");

  cr_assert(ip_check(network_ip, mask, ip_to_check));
}

Test(generate_random_ip, generates_different_ips) {
  srand(42);

  u_int32_t ip1 = generate_random_ip();
  u_int32_t ip2 = generate_random_ip();

  cr_assert_neq(ip1, ip2);
}
