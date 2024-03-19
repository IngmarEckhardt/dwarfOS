
#ifndef COLOSSAL_CAVE_ADVENTURE_INPUTQUEUE_H
#define COLOSSAL_CAVE_ADVENTURE_INPUTQUEUE_H

#include <stdint.h>

#ifndef INPUT_BUFFER_START_SIZE
#define INPUT_BUFFER_START_SIZE 7
#endif
/**
 * InputQueue is a circular buffer data structure for storing uint8_t items as stdin implementation.
 * It provides enqueue, dequeue, and get_char operations, also dynamically resizing the buffer.
 */
typedef struct InputQueue InputQueue;

struct InputQueue{
    uint8_t head;
    uint8_t tail;
    uint8_t size;
    uint8_t * buffer;

    void (* enqueue)(uint8_t item, InputQueue * inputQueue);
    /**
     * Function to remove an item from the queue.
     * Returns the removed item, or -1 if the queue is empty.
     */
    int16_t (* dequeue)(InputQueue * inputQueue);

    /**
    * Function to get a character from the queue.
    * This function is blocking as a normal terminal stdin. If no sign is available it will pause the execution of your
    * program.
    */
    int16_t (* get_char)(InputQueue * inputQueue);
};

InputQueue * dOS_initInputQueue(void);

#endif //COLOSSAL_CAVE_ADVENTURE_INPUTQUEUE_H
