#include <dwarf-os/input_queue.h>
#include <stdlib.h>
#include <string.h>
#ifndef TEST_NON_AVR
#include <avr/sleep.h>
#endif

void increaseSize(InputQueue * inputQueue);

void decreaseSize(InputQueue * inputQueue);

uint8_t countElementsInQueue(InputQueue * inputQueue);

void enqueue(uint8_t item, InputQueue * inputQueue) {

    uint8_t nextTail = (inputQueue->tail + 1) % inputQueue->size;
    if (nextTail == inputQueue->head) {
        uint8_t oldSize = inputQueue->size;
        increaseSize(inputQueue);
        if (inputQueue->size > oldSize) {
            nextTail = (inputQueue->tail + 1) % inputQueue->size;
        } else {
            return;
        }
    }
    inputQueue->buffer[inputQueue->tail] = item;
    inputQueue->tail = nextTail;
}

int16_t dequeue(InputQueue * inputQueue) {

    if (inputQueue->head == inputQueue->tail) { return -1; }
    uint8_t item = inputQueue->buffer[inputQueue->head];
    inputQueue->buffer[inputQueue->head] = 0;
    inputQueue->head = (inputQueue->head + 1) % inputQueue->size;

    if (inputQueue->size > INPUT_BUFFER_START_SIZE && countElementsInQueue(inputQueue) < inputQueue->size / 4) {
        decreaseSize(inputQueue);
    }
    return item;
}

int16_t getCharacter(InputQueue * inputQueue, uint8_t enableSleep) {
    int16_t nextChar = inputQueue->dequeue(inputQueue);
    while (nextChar == -1) {
#ifndef TEST_NON_AVR
        if (enableSleep) { sleep_mode();}
#endif
        nextChar = inputQueue->dequeue(inputQueue);
    }
    return nextChar;
}


InputQueue * dOS_initInputQueue(void) {
    InputQueue * queue = malloc(sizeof(InputQueue));
    if (queue == NULL) { return NULL; }
    else {
        queue->head = 0;
        queue->tail = 0;
        queue->size = INPUT_BUFFER_START_SIZE;
        queue->buffer = malloc(INPUT_BUFFER_START_SIZE * sizeof(uint8_t));
        if (queue->buffer == NULL) {
            free(queue);
            return NULL;
        }
        queue->enqueue = enqueue;
        queue->dequeue = dequeue;
        queue->get_char = getCharacter;
        return queue;
    }
}

void increaseSize(InputQueue * inputQueue) {
    if (inputQueue->size >= UINT8_MAX / 2) {
        return;
    }
    uint8_t newSize = inputQueue->size * 2;

    uint8_t * newBuffer = malloc(newSize * sizeof(uint8_t));
    if (newBuffer == NULL) {
        return;
    } else {
        memcpy(newBuffer, inputQueue->buffer, inputQueue->tail);
    }

    if (inputQueue->head > 0) {
        uint8_t amountOfBytesToMove = inputQueue->size - inputQueue->head;
        uint8_t newHead = (inputQueue->size + inputQueue->head);
        memcpy(newBuffer + newHead,
               inputQueue->buffer + inputQueue->head,
               amountOfBytesToMove);
        inputQueue->head = newHead;
    }
    inputQueue->size = newSize;
    free(inputQueue->buffer);
    inputQueue->buffer = newBuffer;
}

void decreaseSize(InputQueue * inputQueue) {
    if (inputQueue->size < INPUT_BUFFER_START_SIZE * 2) {
        return;
    }
    uint8_t newSize = inputQueue->size / 2;

    uint8_t * newBuffer = malloc(newSize * sizeof(uint8_t));
    if (newBuffer == NULL) { return; }

    if (inputQueue->tail > inputQueue->head) {
        uint8_t amountOfBytesToMove = inputQueue->tail - inputQueue->head;

        memcpy(newBuffer,
               inputQueue->buffer + inputQueue->head,
               amountOfBytesToMove);
        inputQueue->head = 0;
        inputQueue->tail = amountOfBytesToMove;
    } else {
        uint8_t amountOfBytesToMove = inputQueue->size - inputQueue->head;
        uint8_t newHead = newSize - amountOfBytesToMove;

        memcpy(newBuffer + newHead,
               inputQueue->buffer + inputQueue->head,
               amountOfBytesToMove);
        inputQueue->head = newHead;
    }
    inputQueue->size = newSize;
    free(inputQueue->buffer);
    inputQueue->buffer = newBuffer;
}

uint8_t countElementsInQueue(InputQueue * inputQueue) {
    if (inputQueue->head <= inputQueue->tail) {
        return inputQueue->tail - inputQueue->head;
    } else {
        return inputQueue->size - inputQueue->head + inputQueue->tail;
    }
}