#include "contacts.h"

DoublyLinkedList* create_linked_list() {
  DoublyLinkedList* list = (DoublyLinkedList*)malloc(sizeof(DoublyLinkedList));
  if (!list) return NULL;
  list->head = NULL;
  list->tail = NULL;
  return list;
}

bool delete_linked_list(DoublyLinkedList** list) {
  if (!list || !*list) return false;
  Node* curr = (*list)->head;
  while (curr) {
    Node* next = curr->next;
    free_contact(curr->contact);
    free(curr);
    curr = next;
  }

  free(*list);
  *list = NULL;
  return true;
}

Node* create_node(Contact* data) {
  Node* node = (Node*)malloc(sizeof(Node));
  if (!node) return NULL;
  node->contact = data;
  node->prev = NULL;
  node->next = NULL;
  return node;
}

void delete_node(DoublyLinkedList* list, Node* node) {
  if (!list || !node) return;

  if (node->prev)
    node->prev->next = node->next;
  else
    list->head = node->next;

  if (node->next)
    node->next->prev = node->prev;
  else
    list->tail = node->prev;

  free(node);
}

bool insert_to_linked_list(DoublyLinkedList* list, Contact* data,
                           comparator compare) {
  if (!list) return false;
  Node* new_node = create_node(data);
  if (!new_node) return false;
  if (list->head == NULL) {
    list->head = new_node;
    list->tail = new_node;
    return true;
  }
  // Вставка в начало
  if (compare(list->head->contact, data) >= 0) {
    new_node->next = list->head;
    if (list->head != NULL) list->head->prev = new_node;
    list->head = new_node;
    return true;
  }
  // Вставка в уонец
  if (compare(list->tail->contact, data) <= 0) {
    new_node->prev = list->tail;
    new_node->prev->next = new_node;
    new_node->next = NULL;
    list->tail = new_node;
    return true;
  }

  Node* tmp = list->head;
  while (tmp->next != NULL && compare(tmp->next->contact, data) <= 0) {
    tmp = tmp->next;
  }
  new_node->next = tmp->next;
  new_node->prev = tmp;
  tmp->next->prev = new_node;
  tmp->next = new_node;
  return true;
}

bool delete_from_linked_list(DoublyLinkedList* list, void* data,
                             comparator compare) {
  if (!list) return false;
  Node* tmp = list->head;
  while (tmp != NULL) {
    if (compare(tmp->contact, data) == 0) {
      if (tmp->prev != NULL && tmp->next != NULL) {  // удаление из середины
        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
      } else if (tmp->prev == NULL &&
                 tmp->next != NULL) {  // удаление из начала
        list->head = tmp->next;
        tmp->next->prev = NULL;
      } else if (tmp->next == NULL && tmp->prev != NULL) {  // удаление из конца
        list->tail = tmp->prev;
        tmp->prev->next = NULL;
      } else {
        list->head = NULL;
        list->tail = NULL;
      }
      free(tmp);
      return true;
    }

    tmp = tmp->next;
  }
  return false;
}

void print_linked_list(DoublyLinkedList* list, void (*print_node)(Contact*)) {
  if (list == NULL) {
    return;
  }
  Node* tmp = list->head;
  while (tmp != NULL) {
    // printf("%d  ", tmp->data);
    print_node(tmp->contact);
    tmp = tmp->next;
  }
  printf("\n");
}

bool remove_front(DoublyLinkedList* list) {
  if (!list || !list->head) return false;
  delete_node(list, list->head);
  return true;
}

void* find_node(Node* head, void* data, comparator compare) {
  if (head == NULL) return NULL;
  Node* tmp = head;
  while (tmp != NULL) {
    if (compare(tmp->contact, data) == 0) {
      return tmp->contact;
    }
    tmp = tmp->next;
  }
  return NULL;
}