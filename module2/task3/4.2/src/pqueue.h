#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stdbool.h"

typedef struct QueueElement {
  size_t priority;
  int data;
  struct QueueElement* next;
  struct QueueElement* prev;
} QueueElement;

typedef struct PriorityQueue {
  QueueElement* head;
  QueueElement* tail;
} PriorityQueue;

PriorityQueue* create_queue();

bool delete_queue(PriorityQueue* queue);

bool enqueue(PriorityQueue* queue, int data, size_t priority);

QueueElement* dequeue(PriorityQueue* queue);

QueueElement* dequeueWithPriority(PriorityQueue* queue, size_t priority);

QueueElement* dequeueWithMinPriority(PriorityQueue* queue, size_t minPriority);

void print_queue(const PriorityQueue* queue);

void print_element(const QueueElement* elem);

int generate_random_priority();

void test_priority_queue();