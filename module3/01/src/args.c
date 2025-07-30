#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARG_SIZE 50

int is_int(const char *s, int *val) {
  char *endptr;
  errno = 0;
  long num = strtol(s, &endptr, 10);

  if (errno == ERANGE) {
    return 0;
  }
  if (endptr == s || (*endptr != '\0' && !isspace((unsigned char)*endptr))) {
    return 0;
  }
  *val = (int)num;
  return 1;
}

int is_float(const char *s, double *val) {
  char *endptr;
  errno = 0;
  double num = strtod(s, &endptr);

  if (errno == ERANGE) {
    return 0;
  }
  if (endptr == s || (*endptr != '\0' && !isspace((unsigned char)*endptr))) {
    return 0;
  }
  *val = num;
  return 1;
}

void print_numbers(char **args, int start, int end) {
  int i_value = 0;
  double f_value = 0;
  for (int i = start; i <= end; i++) {
    int res_f = is_float(args[i], &f_value);
    int res_i = is_int(args[i], &i_value);
    if (res_i) {
      printf("PID: %d %d %d\n", getpid(), i_value, (i_value) * 2);
    } else if (res_f) {
      printf("PID: %d %.3lf %.3lf\n", getpid(), f_value, (f_value) * 2);
    } else {
      printf("PID: %d %s\n", getpid(), args[i]);
    }
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Слишком мало аргументов\n");
    return 1;
  }

  int num_args = argc - 1;
  int mid = num_args / 2;
  pid_t pid = fork();
  if (pid < 0) {
    perror("Ошибка fork");
  }

  if (pid > 0) {
    print_numbers(argv, 1, mid);
    wait(NULL);
  } else if (pid == 0) {
    print_numbers(argv, mid + 1, argc - 1);
    exit(EXIT_SUCCESS);
  }

  return EXIT_SUCCESS;
}