#include <time.h>

#include "contacts.h"

int rand_range(int min, int max);

// Функция для генерации случайной строки
void rand_string(char* dest, size_t length, const char** options,
                 size_t options_count);

Contact* create_random_contact();