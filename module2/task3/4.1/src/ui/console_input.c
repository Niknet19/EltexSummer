#include "console_input.h"

char* read_string(const char* prompt) {
  printf("%s", prompt);
  char buffer[MAXSTRINGSIZE];
  fgets(buffer, MAXSTRINGSIZE, stdin);
  buffer[strcspn(buffer, "\n")] = '\0';

  char* str = malloc(strlen(buffer) + 1);
  strcpy(str, buffer);
  return str;
}

PhoneNumber* update_phones(size_t* count, PhoneNumber* phones) {
  int choice;
  size_t index;
  char buffer[MAXSTRINGSIZE];

  while (1) {
    printf("\nТекущие телефонные номера:\n");
    if (*count == 0) {
      printf("Список пуст\n");
    } else {
      for (size_t i = 0; i < *count; i++) {
        printf("%zu. %s\n", i + 1, phones[i].number);
      }
    }

    printf("\nВыберите действие:\n");
    printf("1. Добавить номер\n");
    printf("2. Изменить номер\n");
    printf("3. Удалить номер\n");
    printf("4. Завершить редактирование\n");
    printf("Ваш выбор: ");
    scanf("%d", &choice);
    getchar();

    switch (choice) {
      case 1:
        if (*count == 0) {
          phones = malloc(STARTLISTSIZE * sizeof(PhoneNumber));
        } else {
          phones = realloc(phones, (*count + 1) * sizeof(PhoneNumber));
        }

        printf("Введите новый номер: ");
        fgets(buffer, MAXSTRINGSIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        strncpy(phones[*count].number, buffer, MAXSTRINGSIZE - 1);
        phones[*count].number[MAXSTRINGSIZE - 1] = '\0';
        (*count)++;
        break;

      case 2:
        if (*count == 0) {
          printf("Список пуст, нечего изменять\n");
          break;
        }

        printf("Введите номер для изменения (1-%zu): ", *count);
        scanf("%zu", &index);
        getchar();

        if (index < 1 || index > *count) {
          printf("Неверный индекс\n");
          break;
        }

        printf("Текущий номер: %s\n", phones[index - 1].number);
        printf("Введите новый номер: ");
        fgets(buffer, MAXSTRINGSIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        strncpy(phones[index - 1].number, buffer, MAXSTRINGSIZE - 1);
        phones[index - 1].number[MAXSTRINGSIZE - 1] = '\0';
        break;

      case 3:
        if (*count == 0) {
          printf("Список пуст, нечего удалять\n");
          break;
        }

        printf("Введите номер для удаления (1-%zu): ", *count);
        scanf("%zu", &index);
        getchar();

        if (index < 1 || index > *count) {
          printf("Неверный индекс\n");
          break;
        }

        for (size_t i = index - 1; i < *count - 1; i++) {
          strcpy(phones[i].number, phones[i + 1].number);
        }

        (*count)--;
        phones = realloc(phones, *count * sizeof(PhoneNumber));
        break;

      case 4:
        return phones;

      default:
        printf("Неверный выбор\n");
    }
  }

  return phones;
}

PhoneNumber* input_phones(size_t* count) {
  printf("Введите количество телефонов: ");
  scanf("%zu", count);
  getchar();

  PhoneNumber* phones = malloc(*count * sizeof(PhoneNumber));
  for (size_t i = 0; i < *count; i++) {
    printf("Телефон %zu: ", i + 1);
    fgets(phones[i].number, MAXSTRINGSIZE, stdin);
    phones[i].number[strcspn(phones[i].number, "\n")] = '\0';
  }
  return phones;
}

Email* input_emails(size_t* count) {
  printf("Введите количество email: ");
  scanf("%zu", count);
  getchar();

  Email* emails = malloc(*count * sizeof(Email));
  for (size_t i = 0; i < *count; i++) {
    printf("Email %zu: ", i + 1);
    fgets(emails[i].email, MAXSTRINGSIZE, stdin);
    emails[i].email[strcspn(emails[i].email, "\n")] = '\0';
  }
  return emails;
}

SocialLink* input_links(size_t* count) {
  printf("Введите количество соц. ссылок: ");
  scanf("%zu", count);
  getchar();

  SocialLink* links = malloc(*count * sizeof(SocialLink));
  for (size_t i = 0; i < *count; i++) {
    printf("Соц. ссылка %zu: ", i + 1);
    fgets(links[i].url, MAXSTRINGSIZE, stdin);
    links[i].url[strcspn(links[i].url, "\n")] = '\0';
  }
  return links;
}

void console_add_contact(ContactList* list) {
  printf("\n=== Добавление нового контакта ===\n");

  char* firstname = read_string("Имя: ");
  char* lastname = read_string("Фамилия: ");
  char* company = read_string("Компания: ");

  size_t phoneCount, emailCount, linkCount;
  PhoneNumber* phones = input_phones(&phoneCount);
  Email* emails = input_emails(&emailCount);
  SocialLink* links = input_links(&linkCount);

  if (add_contact(list, firstname, lastname, company, phones, emails, links,
                  phoneCount, emailCount, linkCount)) {
    printf("Контакт успешно добавлен!\n");
  } else {
    printf("Ошибка при добавлении контакта!\n");
    free(phones);
    free(emails);
    free(links);
  }
  free(firstname);
  free(lastname);
  free(company);
}

ssize_t console_find_by_name(ContactList* list, Contact** contact_to_find) {
  printf("Введите Имя и Фамилию контакта для изменения: ");
  char firstname_buffer[MAXSTRINGSIZE];
  char lastname_buffer[MAXSTRINGSIZE];
  printf("Имя:");
  fgets(firstname_buffer, MAXSTRINGSIZE, stdin);
  firstname_buffer[strcspn(firstname_buffer, "\n")] = '\0';
  printf("Фамилия:");
  fgets(lastname_buffer, MAXSTRINGSIZE, stdin);
  lastname_buffer[strcspn(lastname_buffer, "\n")] = '\0';

  ssize_t id = find_contact_by_name(list, contact_to_find, firstname_buffer,
                                    lastname_buffer);
  if (id == -1) printf("Контакт не найден!\n");
  return id;
}

void console_update_contact(ContactList* list) {
  printf("\n=== Изменение контакта ===\n");
  print_contact_list(list);
  Contact* contact = NULL;
  ssize_t id = console_find_by_name(list, &contact);
  if (id == -1) return;
  printf("Оставьте поле пустым, если не хотите его изменять\n");

  char* firstname = read_string("Новое имя: ");
  char* lastname = read_string("Новая фамилия: ");
  char* company = read_string("Новая компания: ");

  size_t phoneCount = contact->phoneCount, emailCount = contact->emailCount,
         linkCount = contact->socialLinkCount;
  PhoneNumber* phones = copy_phone_numbers(contact->phones, phoneCount);
  Email* emails = NULL;
  SocialLink* links = NULL;

  printf("Хотите изменить телефоны? (y/n): ");
  if (getchar() == 'y') {
    getchar();
    phones = update_phones(&phoneCount, phones);
  } else {
    getchar();
  }

  printf("Хотите изменить emails? (y/n): ");
  if (getchar() == 'y') {
    getchar();
    emails = input_emails(&emailCount);
  } else {
    getchar();
  }

  printf("Хотите изменить соц. ссылки? (y/n): ");
  if (getchar() == 'y') {
    getchar();
    links = input_links(&linkCount);
  } else {
    getchar();
  }

  if (update_contact(list, id, strlen(firstname) > 0 ? firstname : NULL,
                     strlen(lastname) > 0 ? lastname : NULL,
                     strlen(company) > 0 ? company : NULL, phones, emails,
                     links, phoneCount, emailCount, linkCount)) {
    printf("Контакт успешно изменен!\n");
  } else {
    printf("Ошибка при изменении контакта!\n");
  }

  free(firstname);
  free(lastname);
  free(company);
}

void console_delete_contact(ContactList* list) {
  printf("\n=== Удаление контакта ===\n");
  print_contact_list(list);
  printf("Введите Имя и Фамилию контакта для удаления: ");
  size_t id = console_find_by_name(list, NULL);

  if (delete_contact(list, id)) {
    printf("Контакт успешно удален!\n");
  } else {
    printf("Ошибка при удалении контакта или контакт не найден!\n");
  }
}

void show_menu() {
  printf("\n=== Меню управления контактами ===\n");
  printf("1. Показать все контакты\n");
  printf("2. Добавить контакт\n");
  printf("3. Изменить контакт\n");
  printf("4. Удалить контакт\n");
  printf("5. Выход\n");
  printf("Выберите действие: ");
}
