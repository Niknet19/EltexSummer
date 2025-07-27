#include "improvedcalc.h"
bool add(double* result, size_t num_args, ...) {
  if (num_args == 0) {
    return false;
  }

  *result = 0.0;
  // printf("HELLO\n");
  va_list args;
  va_start(args, num_args);

  for (size_t i = 0; i < num_args; i++) {
    *result += va_arg(args, double);
  }

  va_end(args);
  return true;
}