#pragma once
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define MAXSTRINGSIZE 128
#define STARTLISTSIZE 1
typedef char String[MAXSTRINGSIZE];

typedef struct {
  String number;
} PhoneNumber;

typedef struct {
  String email;
} Email;

typedef struct {
  String url;
} SocialLink;

typedef struct Contact {
  size_t id;
  String firstName;
  String lastName;

  String company;

  PhoneNumber *phones;
  size_t phoneCount;

  Email *emails;
  size_t emailCount;

  SocialLink *socialLinks;
  size_t socialLinkCount;
} Contact;

typedef struct ContactList {
  Contact *contacts;
  size_t size;
  size_t capacity;
  size_t next_id;
} ContactList;

ssize_t find_contact_by_name(ContactList *list, Contact **contact_to_find,
                             const char *firstname, const char *lastname);

bool add_contact(ContactList *list, char *firstname, const char *lastname,
                 const char *company, PhoneNumber *phones, Email *emails,
                 SocialLink *links, size_t phoneCount, size_t emailCount,
                 size_t socialLinkCount);
bool update_contact(ContactList *list, size_t id, char *firstname,
                    const char *lastname, const char *company,
                    PhoneNumber *phones, Email *emails, SocialLink *links,
                    size_t phoneCount, size_t emailCount,
                    size_t socialLinkCount);
bool delete_contact(ContactList *list, size_t id_to_delete);

void print_contact_list(ContactList *list);

void print_contact(Contact *contact);

ContactList *create_list(size_t size);

PhoneNumber *copy_phone_numbers(const PhoneNumber *source, size_t count);

void free_list(ContactList *);

void free_contact(Contact *);