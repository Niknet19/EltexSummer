#include "console_input.h"

#ifdef CRITERION_TESTING

#else
int main() {
  ContactList* list = create_list(STARTLISTSIZE);
  int choice = 0;

  while (choice != 5) {
    show_menu();
    scanf("%d", &choice);
    getchar();

    switch (choice) {
      case 1:
        print_contact_list(list);
        break;
      case 2:
        console_add_contact(list);
        break;
      case 3:
        console_update_contact(list);
        break;
      case 4:
        console_delete_contact(list);
        break;
      case 5:
        printf("Выход...\n");
        break;
      default:
        printf("Неверный выбор!\n");
    }
  }

  free_list(list);
  return 0;
}
#endif