#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  int res = 0;
  for (size_t i = 1; i < argc; i++) {
    res += atoi(argv[i]);
  }

  printf("Результат сложения: %d", res);
  return 0;
}