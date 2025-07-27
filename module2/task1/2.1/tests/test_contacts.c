#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../src/console_input.h"

void setup_contact_data(void);

void teardown_contact_data(void);

typedef struct {
  PhoneNumber *phones;
  size_t phoneCount;

  Email *emails;
  size_t emailCount;

  SocialLink *socialLinks;
  size_t socialLinkCount;
} TestContactData;

TestSuite(contact_list, .init = setup_contact_data,
          .fini = teardown_contact_data);

static TestContactData *test_data = NULL;

void setup_contact_data(void) {
  test_data = malloc(sizeof(TestContactData));
  test_data->emailCount = 3;
  test_data->phoneCount = 3;
  test_data->socialLinkCount = 3;
  test_data->phones =
      (PhoneNumber *)malloc(test_data->phoneCount * sizeof(PhoneNumber));
  test_data->emails = (Email *)malloc(test_data->emailCount * sizeof(Email));
  test_data->socialLinks =
      (SocialLink *)malloc(test_data->socialLinkCount * sizeof(SocialLink));
  for (size_t i = 0; i < test_data->phoneCount; i++) {
    strcpy(test_data->phones[i].number, "+123456789");
  }
  for (size_t i = 0; i < test_data->emailCount; i++) {
    strcpy(test_data->emails[i].email, "bbaaaa@mail.com");
  }
  for (size_t i = 0; i < test_data->socialLinkCount; i++) {
    strcpy(test_data->socialLinks[i].url, "@jfjjfjfjfjf");
  }
}

void teardown_contact_data(void) {
  free(test_data->emails);
  free(test_data->phones);
  free(test_data->socialLinks);
}

Test(contact_list, create_list_valid_size) {
  ContactList *list = create_list(5);
  cr_assert_not_null(list, "List should be created");
  cr_assert_eq(list->size, 5, "List size should be 5");
  cr_assert_eq(list->capacity, 0, "Initial capacity should be 0");
  free_list(list);
}

Test(contact_list, create_list_zero_size) {
  ContactList *list = create_list(0);
  cr_assert_null(list, "List with size 0 should return NULL");
}

// Тесты для add_contact()
Test(contact_list, add_first_contact) {
  ContactList *list = create_list(1);
  bool result = add_contact(list, "John", "Doe", "Company", test_data->phones,
                            test_data->emails, test_data->socialLinks,
                            test_data->phoneCount, test_data->emailCount,
                            test_data->socialLinkCount);

  cr_assert(result, "Contact should be added successfully");
  cr_assert_eq(list->capacity, 1, "Capacity should be 1 after adding");
  cr_assert_str_eq(list->contacts[0].firstName, "John",
                   "First name should match");
  free_list(list);
}

Test(contact_list, update_contact) {
  ContactList *list = create_list(1);
  bool result = add_contact(list, "John", "Doe", "Company", test_data->phones,
                            test_data->emails, test_data->socialLinks,
                            test_data->phoneCount, test_data->emailCount,
                            test_data->socialLinkCount);
  free_list(list);
}
