#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../src/persmissions.h"

Test(parse_numeric, valid_input) {
  mode_t mode = parse_numeric_permissions("755");
  cr_assert_eq(mode & 0777, 0755, "Expected 0755 for '755'");

  mode = parse_numeric_permissions("644");
  cr_assert_eq(mode & 0777, 0644, "Expected 0644 for '644'");
}

Test(parse_numeric, invalid_input, .exit_code = EXIT_FAILURE) {
  parse_numeric_permissions("abc");
}

Test(parse_symbolic, valid_input) {
  mode_t mode = parse_symbolic_permissions("rwxr-xr-x");
  cr_assert_eq(mode & 0777, 0755, "Expected 0755 for 'rwxr-xr-x'");

  mode = parse_symbolic_permissions("rw-r--r--");
  cr_assert_eq(mode & 0777, 0644, "Expected 0644 for 'rw-r--r--'");
}

Test(parse_symbolic, invalid_input, .exit_code = EXIT_FAILURE) {
  parse_symbolic_permissions("invalid");  // Должен завершиться с ошибкой
}

Test(modify, add_permissions) {
  mode_t mode = 0644;
  modify_permissions(&mode, "u+x");
  cr_assert_eq(mode & 0777, 0744, "Expected u+x to give 0744");

  modify_permissions(&mode, "g+w");
  cr_assert_eq(mode & 0777, 0764, "Expected g+w to give 0764");
}

Test(modify, remove_permissions) {
  mode_t mode = 0755;
  modify_permissions(&mode, "o-x");
  cr_assert_eq(mode & 0777, 0754, "Expected o-x to give 0754");
}

Test(modify, set_exact_permissions) {
  mode_t mode = 0777;
  modify_permissions(&mode, "u=rw");
  cr_assert_eq(mode & 0777, 0677, "Expected u=rw to give 0677");
}

Test(modify, set_other_exact_permissions) {
  mode_t mode = 0777;
  modify_permissions(&mode, "a=");
  cr_assert_eq(mode, 0, "Expected a= to give 0");
}

Test(file_perms, get_permissions) {
  // Создаем временный файл
  FILE *tmp = fopen("testfile.tmp", "w");
  fclose(tmp);
  chmod("testfile.tmp", 0644);

  mode_t mode = 0;
  get_file_permissions("testfile.tmp", &mode);
  cr_assert_eq(mode & 0777, 0644, "Expected 0644 for test file");

  remove("testfile.tmp");
}