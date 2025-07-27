#include "contacts.h"

int contact_compare_by_id(Contact *c1, Contact *c2) {
  if (c1->id < c2->id) return -1;
  if (c1->id > c2->id) return 1;
  return 0;
}
int contact_compare_with_id(Contact *c, size_t id) {
  if (c->id < id) return -1;
  if (c->id > id) return 1;
  return 0;
}
int contact_compare_by_name(Contact *c1, Contact *c2) {
  return strcmp(c1->firstName, c2->firstName);
}

int contact_compare_by_name_str(Contact *c1, const char *str) {
  return strcmp(c1->firstName, str);
}

bool delete_contact(ContactList *list, size_t id_to_delete) {
  if (list == NULL || list->size <= 0) {
    return false;
  }

  if (delete_from_linked_list(list->contacts, (size_t)id_to_delete,
                              contact_compare_with_id)) {
    list->size--;
    return true;
  } else
    return false;
}

bool add_contact(ContactList *list, char *firstname, const char *lastname,
                 const char *company, PhoneNumber *phones, Email *emails,
                 SocialLink *links, size_t phoneCount, size_t emailCount,
                 size_t socialLinkCount) {
  if (list == NULL) list = create_list(0);

  Contact *newContact = (Contact *)malloc(sizeof(Contact));
  newContact->id = list->next_id++;
  strncpy(newContact->firstName, firstname, sizeof(newContact->firstName) - 1);
  strncpy(newContact->lastName, lastname, sizeof(newContact->lastName) - 1);
  strncpy(newContact->company, company, sizeof(newContact->company) - 1);
  newContact->phones = phones;
  newContact->emails = emails;
  newContact->socialLinks = links;
  newContact->phoneCount = phoneCount;
  newContact->emailCount = emailCount;
  newContact->socialLinkCount = socialLinkCount;
  if (insert_to_linked_list(list->contacts, newContact,
                            contact_compare_by_name)) {
    list->size++;
    return true;
  } else
    return false;
}

bool update_contact(ContactList *list, size_t id_to_update, char *firstname,
                    const char *lastname, const char *company,
                    PhoneNumber *phones, Email *emails, SocialLink *links,
                    size_t phoneCount, size_t emailCount,
                    size_t socialLinkCount) {
  if (list == NULL) {
    return false;
  }
  Contact *contact_ptr =
      find_node(list->contacts->head, id_to_update, contact_compare_with_id);

  if (contact_ptr == NULL) {
    return false;
  }

  if (firstname != NULL) {
    strncpy(contact_ptr->firstName, firstname,
            sizeof(contact_ptr->firstName) - 1);
    contact_ptr->firstName[sizeof(contact_ptr->firstName) - 1] = '\0';
  }
  if (lastname != NULL) {
    strncpy(contact_ptr->lastName, lastname, sizeof(contact_ptr->lastName) - 1);
    contact_ptr->lastName[sizeof(contact_ptr->lastName) - 1] = '\0';
  }
  if (company != NULL) {
    strncpy(contact_ptr->company, company, sizeof(contact_ptr->company) - 1);
    contact_ptr->company[sizeof(contact_ptr->company) - 1] = '\0';
  }

  if (phones != NULL) {
    free(contact_ptr->phones);
    contact_ptr->phones = phones;
    contact_ptr->phoneCount = phoneCount;
  }
  if (emails != NULL) {
    free(contact_ptr->emails);
    contact_ptr->emails = emails;
    contact_ptr->emailCount = emailCount;
  }
  if (links != NULL) {
    free(contact_ptr->socialLinks);
    contact_ptr->socialLinks = links;
    contact_ptr->socialLinkCount = socialLinkCount;
  }

  return true;
}

ContactList *create_list() {
  ContactList *list = (ContactList *)malloc(sizeof(ContactList));
  list->contacts = create_linked_list();
  list->size = 0;
  list->next_id = 0;
  return list;
}

void print_contact(Contact *contact) {
  if (contact == NULL) {
    printf("Contact is NULL.\n");
    return;
  }
  printf("Id: %zu\n", contact->id);
  printf("First Name: %s\n", contact->firstName);
  printf("Surname: %s\n", contact->lastName);
  printf("Workplace: %s\n", contact->company);
  printf("Numbers:\n");
  for (size_t i = 0; i < contact->phoneCount; i++) {
    printf("%s; ", contact->phones[i].number);
  }
  printf("\nSocial links:\n");
  for (size_t i = 0; i < contact->socialLinkCount; i++) {
    printf("%s; ", contact->socialLinks[i].url);
  }
  printf("\nEmails:\n");
  for (size_t i = 0; i < contact->emailCount; i++) {
    printf("%s; ", contact->emails[i].email);
  }
}

void print_contact_list(ContactList *list) {
  if (list->size == 0) {
    printf("Пусто.\n");
    return;
  }
  print_linked_list(list->contacts, print_contact);
}

ssize_t find_contact_by_name(ContactList *list, Contact **contact_to_find,
                             const char *firstName, const char *lastName) {
  ssize_t id = -1;
  if (list == NULL || firstName == NULL || lastName == NULL) {
    return id;
  }
  Contact *found =
      find_node(list->contacts->head, firstName, contact_compare_by_name_str);
  if (contact_to_find != NULL) *contact_to_find = found;
  if (found != NULL) id = found->id;
  return id;
}

void free_contact(Contact *contact) {
  free(contact->emails);
  free(contact->phones);
  free(contact->socialLinks);
  free(contact);
}

void free_list(ContactList *list) {
  delete_linked_list(&(list->contacts));
  free(list);
}

PhoneNumber *copy_phone_numbers(const PhoneNumber *source, size_t count) {
  if (source == NULL || count == 0) {
    return NULL;
  }

  PhoneNumber *copy = (PhoneNumber *)malloc(count * sizeof(PhoneNumber));
  if (copy == NULL) {
    return NULL;
  }

  for (size_t i = 0; i < count; i++) {
    strncpy(copy[i].number, source[i].number, MAXSTRINGSIZE - 1);
    copy[i].number[MAXSTRINGSIZE - 1] = '\0';
  }

  return copy;
}
