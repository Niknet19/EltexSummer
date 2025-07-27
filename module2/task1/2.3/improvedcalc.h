#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#define EPSILON 1e-10
#define OPERATION_NUMBER 4

bool is_zero(double x);

typedef bool (*operation_func_ptr)(double*, size_t, ...);

bool add(double* result, size_t num_args, ...);
bool sub(double* result, size_t num_args, ...);
bool multiply(double* result, size_t num_args, ...);
bool division(double* result, size_t num_args, ...);