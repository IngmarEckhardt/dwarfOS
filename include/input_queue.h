
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
    volatile uint8_t * clock;
    uint8_t adjustValue;

    void (* enqueue)(uint8_t item, InputQueue * inputQueue);
    /**
     * Function to remove an item from the queue.
     * Returns the removed item, or -1 if the queue is empty.
     */
    int16_t (* dequeue)(InputQueue * inputQueue);

    /**
    * Function to get a character from the queue.
    * This function checks the clock after every call to give the serial connection a delay to receive additional signs.
    * If the clock has a difference of 2 ticks or more, the function will return -1 and set the EOF flag.
    */
    int16_t (* get_char)(InputQueue * inputQueue);
};

// @param clock A pointer to a clock variable, after every call to get_char, the clock will be checked to see if the
// difference between the current time and the time of the last call to give the serial connection a delay to receive
// additional signs. If the clock has a difference of 2 ticks or more, the function will return -1 and set the
// EOF flag.
InputQueue * cca_initInputQueue(void);

#endif //COLOSSAL_CAVE_ADVENTURE_INPUTQUEUE_H
