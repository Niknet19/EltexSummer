#include "interpreter.h"

int parse_commands(const char* command, char parsed[MAX_COMMANDS][MAX_ARG_SIZE],
                   char* input_file, char* output_file) {
  input_file[0] = '\0';
  output_file[0] = '\0';
  for (int i = 0; i < MAX_COMMANDS; i++) {
    parsed[i][0] = '\0';
  }

  int cmd_count = 0;
  char temp_command[strlen(command) + 1];
  strcpy(temp_command, command);

  char* commands[MAX_COMMANDS];
  char* saveptr;

  char* token = strtok_r(temp_command, "|", &saveptr);
  while (token != NULL && cmd_count < MAX_COMMANDS) {
    commands[cmd_count] = token;
    cmd_count++;
    token = strtok_r(NULL, "|", &saveptr);
  }

  for (int i = 0; i < cmd_count; i++) {
    char* cmd = commands[i];
    char* in_pos = strstr(cmd, "<");
    char* out_pos = strstr(cmd, ">");

    if (in_pos != NULL) {
      *in_pos = '\0';
      char* file = in_pos + 1;
      while (*file == ' ') file++;
      char* end = file + strlen(file) - 1;
      while (end > file && *end == ' ') end--;
      *(end + 1) = '\0';
      char* newline = strchr(file, '\n');
      if (newline) *newline = '\0';

      strncpy(input_file, file, MAX_FILENAME_LEN - 1);
      input_file[MAX_FILENAME_LEN - 1] = '\0';
    }

    if (out_pos != NULL) {
      *out_pos = '\0';
      char* file = out_pos + 1;
      while (*file == ' ') file++;
      char* end = file + strlen(file) - 1;
      while (end > file && *end == ' ') end--;
      *(end + 1) = '\0';
      char* newline = strchr(file, '\n');
      if (newline) *newline = '\0';

      strncpy(output_file, file, MAX_FILENAME_LEN - 1);
      output_file[MAX_FILENAME_LEN - 1] = '\0';
    }

    char* start = cmd;
    while (*start == ' ') start++;

    char* end = start + strlen(start) - 1;
    while (end > start && *end == ' ') end--;
    *(end + 1) = '\0';

    strncpy(parsed[i], start, MAX_ARG_SIZE - 1);
    parsed[i][MAX_ARG_SIZE - 1] = '\0';
  }

  return cmd_count;
}

void execute_pipeline(char* input) {
  char parsed[MAX_COMMANDS][MAX_ARG_SIZE];
  char input_file[MAX_FILENAME_LEN];
  char output_file[MAX_FILENAME_LEN];
  int num_commands = parse_commands(input, parsed, input_file, output_file);

  int pipes[MAX_PIPES][2];
  pid_t pids[MAX_PIPES];
  for (int i = 0; i < num_commands - 1; i++) {
    if (pipe(pipes[i]) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < num_commands; i++) {
    pids[i] = fork();
    if (pids[i] == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (pids[i] == 0) {
      if (i == 0 && input_file[0] != '\0') {
        int fd = open(input_file, O_RDONLY);
        if (fd == -1) {
          // fprintf(stderr, "%s\n", input_file);
          perror("open");
          exit(EXIT_FAILURE);
        }
        if (dup2(fd, STDIN_FILENO) == -1) {
          perror("dup2");
          _exit(1);
        }
        close(fd);
      }
      if (i > 0) {
        dup2(pipes[i - 1][0], STDIN_FILENO);  // 0 - читать 1 - писать в пайп
      }
      if (i < num_commands - 1) {
        dup2(pipes[i][1], STDOUT_FILENO);  //  01 01
      } else if (output_file[0] != '\0') {
        int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
          perror("open output");
          exit(EXIT_FAILURE);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
      }

      for (int j = 0; j < num_commands; j++) {
        close(pipes[j][0]);
        close(pipes[j][1]);
      }

      execute_command(parsed[i]);
    }
  }
  for (int i = 0; i < num_commands - 1; i++) {
    close(pipes[i][0]);
    close(pipes[i][1]);
  }
  for (int i = 0; i < num_commands; i++) {
    wait(NULL);
  }
}

void execute_command(char* command) {
  char* args[MAX_ARG_SIZE];
  char* token;
  int i = 0;

  token = strtok(command, " \n");
  while (token != NULL && i < MAX_ARG_SIZE - 1) {
    args[i++] = token;
    token = strtok(NULL, " \n");
  }
  args[i] = NULL;

  if (execvp(args[0], args) == -1) {
    perror("Ошибка exec");
    _exit(EXIT_FAILURE);
  }
}

int main() {
  char input[MAX_INPUT_SIZE];

  while (1) {
    printf("Введите команду (или 'exit' для выхода): ");

    if (fgets(input, sizeof(input), stdin) == NULL) {
      perror("Ошибка чтения ввода");
      continue;
    }

    if (strcmp(input, "exit\n") == 0) {
      break;
    }

    // printf("Input file: %s\n", input_file[0] ? input_file : "NULL");
    // printf("Output file: %s\n", output_file[0] ? output_file : "NULL");
    // printf("Commands:\n");
    // for (int i = 0; i < cmd_count; i++) {
    //   printf("%d: %s\n", i, parsed[i]);
    // }
    execute_pipeline(input);
    // execute_command(input);
  }

  return EXIT_SUCCESS;
}