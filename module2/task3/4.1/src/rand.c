#include "rand.h"

int rand_range(int min, int max) { return rand() % (max - min + 1) + min; }

// Функция для генерации случайной строки
void rand_string(char* dest, size_t length, const char** options,
                 size_t options_count) {
  if (options_count > 0) {
    strcpy(dest, options[rand() % options_count]);
  } else {
    // Генерация простой случайной строки если нет вариантов
    for (size_t i = 0; i < length - 1; i++) {
      dest[i] = 'a' + rand() % 26;
    }
    dest[length - 1] = '\0';
  }
}

Contact* create_random_contact() {
  static int initialized = 0;
  if (!initialized) {
    srand(time(NULL));
    initialized = 1;
  }
  static size_t id = 0;
  Contact* contact = (Contact*)malloc(sizeof(Contact));
  if (!contact) return NULL;

  // Варианты для случайного выбора
  const char* companies[] = {"НГТУ",      "Google", "Apple",
                             "Microsoft", "Яндекс", "Сбер"};
  const char* firstNames[] = {"Никита",    "Алексей", "Мария",
                              "Екатерина", "Дмитрий", "Виктор"};
  const char* lastNames[] = {"Жуков",   "Петров",  "Сидорова",
                             "Иванова", "Смирнов", "Кузнецов"};

  // Заполнение полей случайными значениями
  rand_string(contact->company, sizeof(contact->company), companies,
              sizeof(companies) / sizeof(companies[0]));
  rand_string(contact->firstName, sizeof(contact->firstName), firstNames,
              sizeof(firstNames) / sizeof(firstNames[0]));
  rand_string(contact->lastName, sizeof(contact->lastName), lastNames,
              sizeof(lastNames) / sizeof(lastNames[0]));

  // Простые числовые поля
  contact->emailCount = 0;
  contact->id = id++;

  contact->emails = NULL;
  contact->phones = NULL;
  contact->socialLinks = NULL;

  return contact;
}