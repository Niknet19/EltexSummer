#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARG_SIZE 100
#define MAX_FILENAME_LEN 100
#define MAX_PIPES 10
#define MAX_COMMANDS MAX_PIPES

void execute_command(char* command);
void execute_pipeline(char* input);
int parse_commands(const char* command, char parsed[MAX_COMMANDS][MAX_ARG_SIZE],
                   char* input_file, char* output_file);