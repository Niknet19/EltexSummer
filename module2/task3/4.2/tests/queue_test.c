#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../src/pqueue.h"

#define TESTSIZE 100

void setup_contact_data(void);

void teardown_contact_data(void);

PriorityQueue* queue;

TestSuite(queue_test, .init = setup_contact_data,
          .fini = teardown_contact_data);

// static TestContactData *test_data = NULL;

void setup_contact_data(void) {
  srand(time(0));
  queue = create_queue();
  //   for (size_t i = 0; i < TESTSIZE; i++) {
  //     enqueue(queue, (int)i, generate_int());
  //   }
  //   for (size_t i = 0; i < TESTSIZE + 2; i++) {
  //     QueueElement* elem = dequeue(queue);
  //   }
}

void teardown_contact_data() { delete_queue(queue); }

Test(queue_test, priority_correct_check) {
  bool res = enqueue(queue, 111, 2000);
  cr_assert_not(res);
}

Test(queue_test, dequeue_test) { cr_assert_not(dequeue(NULL)); }
Test(queue_test, exact_dequeue_test) {
  cr_assert_not(dequeueWithPriority(NULL, 100));
}
Test(queue_test, min_dequeue_test) {
  cr_assert_not(dequeueWithMinPriority(NULL, 100));
}

Test(queue_test, enqueue_test) {
  PriorityQueue* q = NULL;
  bool res = enqueue(q, 111, 10);
  cr_assert_not(res);
}

Test(queue_test, simple_priority_check1) {
  enqueue(queue, 111, 100);
  enqueue(queue, 111, 50);
  enqueue(queue, 222, 10);
  cr_assert_eq(222, dequeue(queue)->data);
}

Test(queue_test, simple_priority_check2) {
  enqueue(queue, 111, 100);
  cr_assert_eq(111, dequeue(queue)->data);
}

Test(queue_test, exact_priority_priority_check1) {
  enqueue(queue, 555, 11);
  enqueue(queue, 444, 15);
  enqueue(queue, 111, 100);
  enqueue(queue, 222, 100);
  enqueue(queue, 333, 10);
  cr_assert_eq(111, dequeueWithPriority(queue, 100)->data);
}

Test(queue_test, exact_priority_priority_check2) {
  enqueue(queue, 555, 11);
  enqueue(queue, 444, 15);
  enqueue(queue, 111, 100);
  enqueue(queue, 222, 100);
  enqueue(queue, 333, 10);
  cr_assert_null(dequeueWithPriority(queue, 200));
}

Test(queue_test, exact_priority_priority_check3) {
  enqueue(queue, 555, 11);
  enqueue(queue, 444, 100);
  enqueue(queue, 111, 100);
  enqueue(queue, 222, 100);
  enqueue(queue, 333, 10);
  cr_assert_eq(444, dequeueWithPriority(queue, 100)->data);
  cr_assert_eq(111, dequeueWithPriority(queue, 100)->data);
  cr_assert_eq(222, dequeueWithPriority(queue, 100)->data);
  cr_assert_null(dequeueWithPriority(queue, 100));
}

Test(queue_test, min_priority_priority_check1) {
  enqueue(queue, 555, 11);
  enqueue(queue, 444, 15);
  enqueue(queue, 111, 110);
  enqueue(queue, 222, 100);
  enqueue(queue, 333, 10);
  cr_assert_eq(333, dequeueWithMinPriority(queue, 15)->data);
  cr_assert_eq(555, dequeueWithMinPriority(queue, 15)->data);
  cr_assert_eq(444, dequeueWithMinPriority(queue, 15)->data);
  cr_assert_null(dequeueWithPriority(queue, 15));
}