#include <unity.h>
#include "dwarf-os/input_queue.h"
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void print_queue(InputQueue * inputQueue);

void setUp(void) {}

void tearDown(void) {
}

void enqueue_initSizeEnqueueTwoElements_hasStillInitSize(void) {
    InputQueue * queue = cca_initInputQueue();
    TEST_ASSERT_NOT_NULL(queue);
    queue->enqueue(1, queue);
    queue->enqueue(2, queue);
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE, queue->size);
    free(queue->buffer);
    free(queue);
}


void enqueueUint8MaxValuesShouldIncreaseSizeAtleastUntilHalfOfUint8Max(void) {
    InputQueue * queue = cca_initInputQueue();
    TEST_ASSERT_NOT_NULL(queue);
    for (int i = 0; i < UINT8_MAX - 1; ++i) {
        queue->enqueue(i, queue);
    }
    TEST_ASSERT_GREATER_OR_EQUAL_UINT8(UINT8_MAX / 2, queue->size);
    free(queue->buffer);
    free(queue);
}

void test_queue_empty_after_init(void) {
    InputQueue * queue = cca_initInputQueue();
    TEST_ASSERT_NOT_NULL(queue);
    TEST_ASSERT_EQUAL(0, queue->head);
    TEST_ASSERT_EQUAL(0, queue->tail);
    free(queue->buffer);
    free(queue);
}

void test_element13enqueueDequeue_expectedBehaviour(void) {
    InputQueue * queue = cca_initInputQueue();
    TEST_ASSERT_NOT_NULL(queue);
    TEST_ASSERT_EQUAL(0, queue->head);
    TEST_ASSERT_EQUAL(0, queue->tail);
    queue->enqueue(13, queue);
    TEST_ASSERT_EQUAL(13, queue->dequeue(queue));
    free(queue->buffer);
    free(queue);
}

void test_queue_resize(void) {
    InputQueue * queue = cca_initInputQueue();
    TEST_ASSERT_NOT_NULL(queue);
    for (int i = 0; i < (INPUT_BUFFER_START_SIZE * 2) - 1; ++i) {
        queue->enqueue(i, queue);
    }
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE * 2, queue->size);
    for (int i = 0; i < (INPUT_BUFFER_START_SIZE * 2) - 1; ++i) {
        queue->enqueue(i, queue);
    }
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE * 4, queue->size);
    for (int i = 0; i < (INPUT_BUFFER_START_SIZE * 4) - 1; ++i) {
        queue->dequeue(queue);
    }
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE, queue->size);
    free(queue->buffer);
    free(queue);
}

void test_dequeue(void) {
    InputQueue * queue = cca_initInputQueue();
    TEST_ASSERT_NOT_NULL(queue);
    queue->enqueue(1, queue);
    queue->enqueue(2, queue);
    uint8_t item = queue->dequeue(queue);
    TEST_ASSERT_EQUAL(1, item);
    free(queue->buffer);
    free(queue);
}

void test_elements_position(void) {
    InputQueue * queue = cca_initInputQueue();
    TEST_ASSERT_NOT_NULL(queue);
    for (uint8_t i = 0; i < 223; ++i) {
        queue->enqueue(i, queue);
    }
    for (uint8_t i = 0; i < 223; ++i) {
        uint8_t item = queue->dequeue(queue);
        TEST_ASSERT_EQUAL(i, item);
    }
    TEST_ASSERT_NOT_NULL(queue);
    for (uint8_t i = 0; i < 223; ++i) {
        queue->enqueue(i, queue);
    }
    for (uint8_t i = 0; i < 223; ++i) {
        uint8_t item = queue->dequeue(queue);
        TEST_ASSERT_EQUAL(i, item);
    }
}

void test_enqueue_when_queue_is_full(void) {
    InputQueue * queue = cca_initInputQueue();
    TEST_ASSERT_NOT_NULL(queue);
    for (int i = 0; i < INPUT_BUFFER_START_SIZE - 1; ++i) {
        queue->enqueue(i, queue);
    }
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE, queue->size);
    queue->enqueue(100, queue);
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE * 2, queue->size);
    free(queue->buffer);
    free(queue);
}

void test_dequeue_when_queue_is_empty(void) {
    InputQueue * queue = cca_initInputQueue();
    TEST_ASSERT_NOT_NULL(queue);
    int16_t item = queue->dequeue(queue);
    TEST_ASSERT_EQUAL(-1, item);
    free(queue->buffer);
    free(queue);
}



void test_increaseSize_when_queue_is_full(void) {
    InputQueue * queue = cca_initInputQueue();
    TEST_ASSERT_NOT_NULL(queue);
    for (int i = 0; i < INPUT_BUFFER_START_SIZE - 1; ++i) {
        queue->enqueue(i, queue);
    }
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE, queue->size);
    queue->enqueue(100, queue);
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE * 2, queue->size);
    free(queue->buffer);
    free(queue);
}

void test_decreaseSize_when_queue_is_less_than_quarter_full(void) {
    InputQueue * queue = cca_initInputQueue();
    TEST_ASSERT_NOT_NULL(queue);
    for (int i = 0; i < (INPUT_BUFFER_START_SIZE * 4) - 1; ++i) {
        queue->enqueue(i, queue);
    }
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE * 4, queue->size);
    for (int i = 0; i < INPUT_BUFFER_START_SIZE * 3; ++i) {
        queue->dequeue(queue);
    }
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE * 2, queue->size);
    for (int i = 0; i < INPUT_BUFFER_START_SIZE - 1; ++i) {
        queue->dequeue(queue);
    }
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE, queue->size);

    free(queue->buffer);
    free(queue);
}

void test_decreaseSize_when_tail_is_less_than_head(void) {
    InputQueue * queue = cca_initInputQueue();
    TEST_ASSERT_NOT_NULL(queue);
    for (int i = 0; i < (INPUT_BUFFER_START_SIZE * 4) - 1; ++i) {
        queue->enqueue(i, queue);
    }
    // head: 0, tail:27, size: 28
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE * 4, queue->size);

    for (int i = 0; i < INPUT_BUFFER_START_SIZE; ++i) {
        queue->dequeue(queue);
    }
    queue->enqueue(1, queue); // head: 7, tail: 0, size: 28
    for (int i = 0; i < (INPUT_BUFFER_START_SIZE * 2) + 1; ++i) {
        queue->dequeue(queue);
    }
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE * 2, queue->size);
    for (int i = 0; i < 4; ++i) {
        queue->dequeue(queue);
    }
    TEST_ASSERT_EQUAL(INPUT_BUFFER_START_SIZE, queue->size);
    free(queue->buffer);
    free(queue);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(enqueue_initSizeEnqueueTwoElements_hasStillInitSize);
    RUN_TEST(enqueueUint8MaxValuesShouldIncreaseSizeAtleastUntilHalfOfUint8Max);
    RUN_TEST(test_queue_empty_after_init);
    RUN_TEST(test_element13enqueueDequeue_expectedBehaviour);
    RUN_TEST(test_queue_resize);
    RUN_TEST(test_dequeue);
    RUN_TEST(test_elements_position);
    RUN_TEST(test_enqueue_when_queue_is_full);
    RUN_TEST(test_dequeue_when_queue_is_empty);
    RUN_TEST(test_increaseSize_when_queue_is_full);
    RUN_TEST(test_decreaseSize_when_queue_is_less_than_quarter_full);
    RUN_TEST(test_decreaseSize_when_tail_is_less_than_head);

    return UNITY_END();
}

void printBits(uint8_t num) {
    for (int bit = 7; bit >= 0; --bit) {
        printf("%d", (num >> bit) & 1);
    }
    printf("\n");
}

void print_queue(InputQueue * inputQueue) {
    printf("Head: %d\n", inputQueue->head);
    printf("Tail: %d\n", inputQueue->tail);
    printf("Elements in the queue: ");
    for (uint8_t i = 0; i < inputQueue->size; i++) {
//        printBits(inputQueue->buffer[i]);
        printf("%d ", inputQueue->buffer[i]);
    }
    printf("\n");
}