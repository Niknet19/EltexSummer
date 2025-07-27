#include <criterion/criterion.h>

#include "../src/simplecalc.h"

Test(add, should_be_true) {
  double res = 100.0;
  cr_assert(add(&res, 4, 1.0, 2.0, 3.0, 5.0, 6.0));
}

Test(add, should_add_numbers) {
  double res;
  cr_assert(add(&res, 3, 1.0, 2.0, 3.0));
  cr_assert_eq(res, 6.0, "Expected 1.0 + 2.0 + 3.0 = 6.0");
}

Test(add, should_fail_on_zero_args) {
  double res;
  cr_assert_not(add(&res, 0));
}

Test(sub, should_subtract_numbers) {
  double res;
  cr_assert(sub(&res, 3, 10.0, 2.0, 3.0));
  cr_assert_eq(res, 5.0, "Expected 10.0 - 2.0 - 3.0 = 5.0");
}

Test(multiply, should_multiply_numbers) {
  double res;
  cr_assert(multiply(&res, 3, 2.0, 3.0, 4.0));
  cr_assert_eq(res, 24.0, "Expected 2.0 * 3.0 * 4.0 = 24.0");
}

Test(division, should_divide_numbers) {
  double res;
  cr_assert(division(&res, 3, 100.0, 5.0, 2.0));
  cr_assert_eq(res, 10.0, "Expected 100.0 / 5.0 / 2.0 = 10.0");
}

Test(division, should_fail_on_zero_division) {
  double res;
  cr_assert_not(division(&res, 2, 10.0, 0.0), "Expected false");
}