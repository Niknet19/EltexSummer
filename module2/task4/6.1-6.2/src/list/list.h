#pragma once
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct Contact Contact;

typedef struct Node {
  Contact* contact;
  struct Node* next;
  struct Node* prev;
} Node;

typedef struct DoublyLinkedList {
  Node* head;
  Node* tail;
} DoublyLinkedList;

typedef int (*comparator)(void*, void*);

DoublyLinkedList* create_linked_list();

bool delete_linked_list(DoublyLinkedList** list);

Node* create_node(Contact* data);

void delete_node(DoublyLinkedList* list, Node* node);

bool insert_to_linked_list(DoublyLinkedList* list, Contact* data, comparator);

bool delete_from_linked_list(DoublyLinkedList* list, void* data, comparator);

void print_linked_list(DoublyLinkedList* list, void (*print_node)(Contact*));

void* find_node(Node* head, void* data, comparator compare);
