#include "contacts.h"
#include "list/list.h"
#include "ui/console_input.h"

// int main() {
//   DoublyLinkedList* list = create_linked_list();
//   for (size_t i = 0; i < 10; i++) {
//     Contact* c = create_random_contact();
//     insert_to_linked_list(list, c, contact_compare_by_name);
//   }
//   print_linked_list(list, print_contact);
//   delete_linked_list(&list);
//   print_linked_list(list, print_contact);
//   return 0;
// }

#ifdef CRITERION_TESTING

#else
int main() {
  ContactList* list = create_list();
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
