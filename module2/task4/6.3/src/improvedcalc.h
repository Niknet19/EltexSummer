#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define EPSILON 1e-10

bool is_zero(double x);

bool add(double* result, size_t num_args, ...);
bool sub(double* result, size_t num_args, ...);
bool multiply(double* result, size_t num_args, ...);
bool division(double* result, size_t num_args, ...);