#pragma once
#include "../contacts.h"

void show_menu();

void console_delete_contact(ContactList* list);

void console_update_contact(ContactList* list);

ssize_t console_find_by_name(ContactList* list, Contact** contact_to_find);

void console_add_contact(ContactList* list);

PhoneNumber* update_phones(size_t* count, PhoneNumber* phones);

SocialLink* input_links(size_t* count);

SocialLink* input_links(size_t* count);

PhoneNumber* input_phones(size_t* count);

char* read_string(const char* prompt);