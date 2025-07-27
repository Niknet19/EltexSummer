#include <dlfcn.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
// #include "improvedcalc.h"

#define OPERATION_NUMBER 4

void get_user_input(size_t* num_args, double* args) {
  printf("Введите количество аргументов (2-4): ");
  scanf("%ld", num_args);

  if (*num_args <= 1 || *num_args > 4) {
    printf("Количество аргументов должно быть от 2 до 4.\n");
    *num_args = 0;
    return;
  }

  printf("Введите %ld аргумента(ов):\n", *num_args);
  for (size_t i = 0; i < *num_args; i++) {
    printf("Аргумент %ld: ", i + 1);
    while (scanf("%lf", &args[i]) != 1) {
      printf("Ошибка ввода. Пожалуйста, введите число: ");
      while (getchar() != '\n');  // Очищаем буфер
    }
  }
}

typedef bool (*operation_func_ptr)(double*, size_t, ...);

typedef struct Operation {
  const char* op_name;
  operation_func_ptr func;
  void* lib_handle;  // указатель на начало библиотеки
} Operation;

int main() {
  Operation operations[] = {
      {"Сложение", NULL, dlopen("./libsum.so", RTLD_LAZY)},
      {"Вычитание", NULL, dlopen("./libsub.so", RTLD_LAZY)},
      {"Умножение", NULL, dlopen("./libmul.so", RTLD_LAZY)},
      {"Деление", NULL, dlopen("./libdiv.so", RTLD_LAZY)}};

  // Проверяем что все загрузилось
  for (int i = 0; i < 4; i++) {
    if (!operations[i].lib_handle) {
      fprintf(stderr, "Ошибка загрузки библиотеки %s: %s\n",
              operations[i].op_name, dlerror());
      for (int j = 0; j < i; j++) {
        dlclose(operations[j].lib_handle);
      }
      return 1;
    }
  }

  // Получаем функции из каждой загруженной библиотеки
  operations[0].func =
      (operation_func_ptr)dlsym(operations[0].lib_handle, "add");
  operations[1].func =
      (operation_func_ptr)dlsym(operations[1].lib_handle, "sub");
  operations[2].func =
      (operation_func_ptr)dlsym(operations[2].lib_handle, "multiply");
  operations[3].func =
      (operation_func_ptr)dlsym(operations[3].lib_handle, "division");

  // Проверка на ошибки загрузки функций
  const char* error;
  for (int i = 0; i < 4; i++) {
    if ((error = dlerror()) != NULL) {
      fprintf(stderr, "Ошибка при загрузке функции %s: %s\n",
              operations[i].op_name, error);
      for (int j = 0; j < 4; j++) {
        dlclose(operations[j].lib_handle);
      }
      return 1;
    }
  }

  int choice;
  double args[4];
  size_t num_args;

  while (1) {
    printf("\nВыберите действие:\n");
    printf("1. Сложение\n");
    printf("2. Вычитание\n");
    printf("3. Умножение\n");
    printf("4. Деление\n");
    printf("5. Выход\n");
    printf("Ваш выбор: ");

    while (scanf("%d", &choice) != 1 || choice < 1 || choice > 5) {
      printf("Ошибка ввода. Пожалуйста, введите число от 1 до 5: ");
      while (getchar() != '\n');
    }

    if (choice == 5) {
      break;
    }

    get_user_input(&num_args, args);
    if (num_args == 0) continue;

    double result = 0;
    bool is_correct;
    is_correct = operations[choice - 1].func(&result, num_args, args[0],
                                             args[1], args[2], args[3]);
    if (is_correct) {
      printf("Результат операции %s : %lf\n", operations[choice - 1].op_name,
             result);
    } else
      printf("Возникла ошибка во время выполнения операции");
  }

  return 0;
}