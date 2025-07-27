#include "contacts.h"

bool delete_contact(ContactList *list, size_t id_to_delete) {
  if (list == NULL || list->size <= 0 || list->capacity <= 0) {
    return false;
  }

  for (size_t i = 0; i < list->capacity; i++) {
    if (list->contacts[i].id == id_to_delete) {
      free_contact(&list->contacts[i]);
      memmove(&list->contacts[i], &list->contacts[i + 1],
              (list->capacity - i - 1) * sizeof(Contact));
      //   for (size_t j = i; j < list->capacity - 1; j++) {
      //     list->contacts[j] = list->contacts[j + 1];
      //   }
      list->capacity--;
      return true;
    }
  }
  return false;
}

bool add_contact(ContactList *list, char *firstname, const char *lastname,
                 const char *company, PhoneNumber *phones, Email *emails,
                 SocialLink *links, size_t phoneCount, size_t emailCount,
                 size_t socialLinkCount) {
  if (list->size <= 0) {
    list = create_list(STARTLISTSIZE);
    if (list == NULL) return false;
  }
  if (list->capacity >= list->size) {
    size_t new_size = list->size * 2;
    Contact *new_contacts =
        (Contact *)realloc(list->contacts, list->size * 2 * sizeof(Contact));
    if (new_contacts == NULL) {
      return false;
    }
    list->contacts = new_contacts;
    list->size = new_size;
  }
  Contact newContact;
  newContact.id = list->next_id++;
  strncpy(newContact.firstName, firstname, sizeof(newContact.firstName) - 1);
  strncpy(newContact.lastName, lastname, sizeof(newContact.lastName) - 1);
  strncpy(newContact.company, company, sizeof(newContact.company) - 1);
  newContact.phones = phones;
  newContact.emails = emails;
  newContact.socialLinks = links;
  newContact.phoneCount = phoneCount;
  newContact.emailCount = emailCount;
  newContact.socialLinkCount = socialLinkCount;
  list->contacts[list->capacity++] = newContact;
  return true;
}

bool update_contact(ContactList *list, size_t id_to_update, char *firstname,
                    const char *lastname, const char *company,
                    PhoneNumber *phones, Email *emails, SocialLink *links,
                    size_t phoneCount, size_t emailCount,
                    size_t socialLinkCount) {
  if (list == NULL || id_to_update >= list->capacity) {
    return false;
  }
  Contact *contact_ptr;
  for (size_t i = 0; i < list->capacity; i++) {
    if (list->contacts[i].id == id_to_update) {
      contact_ptr = &list->contacts[i];
      break;
    }
  }

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

ContactList *create_list(size_t size) {
  if (size <= 0) return NULL;
  ContactList *list = (ContactList *)malloc(sizeof(ContactList));
  list->contacts = (Contact *)malloc(size * sizeof(Contact));
  list->size = size;
  list->capacity = 0;
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
  printf("\nContact List Information:\n");
  for (size_t i = 0; i < list->capacity; i++) {
    print_contact(&(list->contacts[i]));
    printf("\n\n");
  }
}

ssize_t find_contact_by_name(ContactList *list, Contact **contact_to_find,
                             const char *firstName, const char *lastName) {
  ssize_t id = -1;
  if (list == NULL || firstName == NULL || lastName == NULL ||
      list->capacity == 0) {
    return id;
  }
  for (size_t i = 0; i < list->capacity; i++) {
    if (strcmp(list->contacts[i].firstName, firstName) == 0 &&
        strcmp(list->contacts[i].lastName, lastName) == 0) {
      id = list->contacts[i].id;
      if (contact_to_find) *contact_to_find = &list->contacts[i];
    }
  }
  return id;
}

void free_contact(Contact *contact) {
  free(contact->emails);
  free(contact->phones);
  free(contact->socialLinks);
}

void free_list(ContactList *list) { free(list->contacts); }

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
