#include "improvedcalc.h"

bool sub(double* result, size_t num_args, ...) {
  if (num_args == 0) {
    return false;
  }

  va_list args;
  va_start(args, num_args);

  *result = va_arg(args, double);

  for (size_t i = 1; i < num_args; i++) {
    *result -= va_arg(args, double);
  }

  va_end(args);
  return true;
}