#include "improvedcalc.h"

bool multiply(double* result, size_t num_args, ...) {
  if (num_args == 0) {
    fprintf(stderr, "Error: No arguments provided.\n");
    return false;
  }

  *result = 1.0;
  va_list args;
  va_start(args, num_args);

  for (size_t i = 0; i < num_args; i++) {
    *result *= va_arg(args, double);
  }

  va_end(args);
  return true;
}