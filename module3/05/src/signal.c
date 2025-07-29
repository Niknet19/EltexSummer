#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

sig_atomic_t counter = 0;
FILE *file = NULL;
#define FILENAME "output.txt"

void handle_signal(int signal) {
  const char *signal_name;
  if (signal == SIGINT) {
    signal_name = "SIGINT";
    counter++;
  } else if (signal == SIGQUIT) {
    signal_name = "SIGQUIT";
  } else
    return;

  if (file == NULL) {
    file = fopen(FILENAME, "a");
    if (file == NULL) {
      perror("fopen");
      exit(EXIT_FAILURE);
    }
  }

  fprintf(file, "Получен и обработан сигнал %s\n", signal_name);
  fflush(file);

  if (counter > 2) {
    fclose(file);
    exit(EXIT_SUCCESS);
  }
}

int main() {
  signal(SIGINT, handle_signal);
  signal(SIGQUIT, handle_signal);
  int c = 0;
  if (file == NULL) {
    file = fopen(FILENAME, "a");
    if (file == NULL) {
      perror("fopen");
      exit(EXIT_FAILURE);
    }
  }
  printf("ID процесса: %d\n", getpid());
  while (1) {
    fprintf(file, "%d\n", c);
    c++;
    sleep(1);
  }
  return EXIT_SUCCESS;
}
