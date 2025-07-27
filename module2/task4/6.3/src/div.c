#include "improvedcalc.h"

bool is_zero(double x) { return fabs(x) < EPSILON; }

bool division(double* result, size_t num_args, ...) {
  if (num_args == 0) {
    return false;
  }

  va_list args;
  va_start(args, num_args);

  *result = va_arg(args, double);

  for (size_t i = 1; i < num_args; i++) {
    double divider = va_arg(args, double);
    if (is_zero(divider)) {
      va_end(args);
      return false;
    }
    *result /= divider;
  }

  va_end(args);
  return true;
}