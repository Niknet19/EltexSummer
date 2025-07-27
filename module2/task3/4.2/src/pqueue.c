#include "pqueue.h"

PriorityQueue* create_queue() {
  PriorityQueue* pqueue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
  pqueue->head = NULL;
  pqueue->tail = NULL;
  return pqueue;
}

int generate_random_priority() { return rand() % 256; }

bool delete_queue(PriorityQueue* queue) {
  if (!queue) return false;

  QueueElement* curr = queue->head;
  while (curr) {
    QueueElement* next = curr->next;
    free(curr);
    curr = next;
  }

  free(queue);
  queue = NULL;
  return true;
}

bool enqueue(PriorityQueue* queue, int data, size_t priority) {
  if (queue == NULL || priority > 255) return false;
  QueueElement* newElement = (QueueElement*)malloc(sizeof(QueueElement));
  newElement->data = data;
  newElement->priority = priority;
  newElement->next = NULL;
  newElement->prev = NULL;

  if (queue->head == NULL) {
    queue->head = queue->tail = newElement;
    return true;
  }

  QueueElement* curr = queue->head;
  while (curr != NULL && curr->priority <= priority) {
    curr = curr->next;
  }

  if (curr == NULL) {
    queue->tail->next = newElement;
    newElement->prev = queue->tail;
    queue->tail = newElement;
  } else if (curr == queue->head) {
    queue->head->prev = newElement;
    newElement->next = queue->head;
    queue->head = newElement;
  } else {
    newElement->next = curr;
    newElement->prev = curr->prev;
    curr->prev->next = newElement;
    curr->prev = newElement;
  }
  return true;
}

QueueElement* dequeue(PriorityQueue* queue) {
  if (queue == NULL || queue->head == NULL) {
    return NULL;
  }

  QueueElement* highestPriorityElement = queue->head;
  queue->head = queue->head->next;

  if (queue->head != NULL) {
    queue->head->prev = NULL;
  } else {
    queue->tail = NULL;
  }

  return highestPriorityElement;
}

QueueElement* dequeueWithPriority(PriorityQueue* queue, size_t priority) {
  if (queue == NULL || queue->head == NULL) {
    return NULL;
  }
  QueueElement* current = queue->head;

  while (current != NULL) {
    if (current->priority == priority) {
      if (current->prev == NULL) {
        queue->head = current->next;
        queue->head->prev = NULL;
      } else if (current->next == NULL) {
        queue->tail = current->prev;
        queue->tail->next = NULL;
      } else {
        current->prev->next = current->next;
        current->next->prev = current->prev;
      }
      return current;
    }
    current = current->next;
  }
  printf("Элемент с приоритетом %zu не найден.\n", priority);
  return NULL;
}

QueueElement* dequeueWithMinPriority(PriorityQueue* queue, size_t minPriority) {
  if (queue == NULL || queue->head == NULL) {
    return NULL;
  }
  QueueElement* current = queue->head;
  while (current != NULL) {
    if (current->priority >= minPriority) {
      if (current->prev == NULL) {
        queue->head = current->next;
        queue->head->prev = NULL;
      } else if (current->next == NULL) {
        queue->tail = current->prev;
        queue->tail->next = NULL;
      } else {
        current->prev->next = current->next;
        current->next->prev = current->prev;
      }
      return current;
    }
    current = current->next;
  }

  printf("Элемент с приоритетом не ниже %zu не найден.\n", minPriority);
  return NULL;
}

void print_element(const QueueElement* elem) {
  if (!elem)
    printf("NULL");
  else
    printf("{data=%d, priority=%zu}", elem->data, elem->priority);
}

void print_queue(const PriorityQueue* queue) {
  printf("Queue: [");
  const QueueElement* curr = queue->head;
  while (curr) {
    print_element(curr);
    if (curr->next) printf(" -> ");
    curr = curr->next;
  }
  printf("]\n");
}

void test_priority_queue() {
  srand(time(NULL));
  printf("=== Тестирование очереди с приоритетами ===\n\n");

  // 1. Создание очереди
  PriorityQueue* queue = create_queue();
  if (!queue) {
    printf("Ошибка: не удалось создать очередь!\n");
    return;
  }
  printf("Создана пустая очередь.\n");

  // 2. Добавление элементов (включая несколько с приоритетом 100)
  printf("\n--- Добавление элементов ---\n");
  for (int i = 0; i < 15; i++) {
    int data = rand() % 1000;
    // Генерируем приоритет, но гарантируем, что 3 элемента будут с priority=100
    size_t priority = (i % 5 == 0) ? 100 : generate_random_priority();
    printf("Добавляем: data=%d, priority=%zu -> ", data, priority);
    if (enqueue(queue, data, priority)) {
      print_queue(queue);
    } else {
      printf("Ошибка добавления!\n");
    }
  }

  // 3. Извлечение элементов с наивысшим приоритетом
  printf("\n--- Извлечение (dequeue) ---\n");
  for (int i = 0; i < 2; i++) {
    QueueElement* elem = dequeue(queue);
    printf("Извлечено: ");
    print_element(elem);
    printf("\n");
    free(elem);
    print_queue(queue);
  }

  // 4. Извлечение по конкретному приоритету (100) — теперь такие элементы точно
  // есть!
  printf("\n--- Извлечение по приоритету (dequeueWithPriority) ---\n");
  size_t target_priority = 100;
  printf("Извлекаем ВСЕ элементы с priority=%zu...\n", target_priority);

  int extracted_count = 0;
  while (1) {
    QueueElement* elem = dequeueWithPriority(queue, target_priority);
    if (!elem) break;

    printf("Извлечено %d: ", ++extracted_count);
    print_element(elem);
    printf("\n");
    free(elem);
    print_queue(queue);
  }

  if (extracted_count == 0) {
    printf("Ошибка: элементы с priority=%zu не найдены!\n", target_priority);
  } else {
    printf("Успешно извлечено %d элементов.\n", extracted_count);
  }

  printf("\n--- Извлечение с min_priority (dequeueWithMinPriority) ---\n");
  for (int i = 0; i < 2; i++) {
    size_t min_priority = generate_random_priority();
    printf("Пробуем извлечь элемент с priority >= %zu...\n", min_priority);
    QueueElement* elem = dequeueWithMinPriority(queue, min_priority);
    if (elem) {
      printf("Успешно: ");
      print_element(elem);
      printf("\n");
      free(elem);
    }
    print_queue(queue);
  }
  printf("\n--- Осталось в очереди ---\n\n");

  print_queue(queue);

  printf("\n--- Очистка очереди ---\n");
  delete_queue(queue);
}