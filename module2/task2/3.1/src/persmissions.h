#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

mode_t parse_numeric_permissions(const char *input);

mode_t parse_symbolic_permissions(const char *input);

void modify_permissions(mode_t *mode, const char *command);

void get_file_permissions(const char *filename, mode_t *mode);
