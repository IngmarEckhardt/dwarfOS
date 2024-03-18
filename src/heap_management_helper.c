#include "heap_management_helper.h"
#include <avr/pgmspace.h>
#include <stdlib.h>

extern char * __brkval;

// we have 16kB Maximum Amount of Ram in an avr, we can force uint16_t_MIN - (uint16_t) memoryAddress into an int16_t result
int16_t getFreeMemory(void) {
    // SP is the stack pointer, this calculation ignores fragmented memory inside the area that is managed from malloc
    // but already freed and the space between bss and malloc heap end, if first allocated memory is freed.
    // There is no guarantee that you can use these fragments if they are too small for the next allocated data structure
    return  __brkval ?  (int16_t)(SP - ((uint16_t) &__brkval)) : (int16_t)(SP - (uint16_t) &__malloc_heap_start);
}

int16_t getStackpointerValue (void) {
    return SP;
}
HeapManagementHelper * dOS_initHeapManagementHelper(void) {
    HeapManagementHelper * helper = malloc(sizeof(HeapManagementHelper));
    if (helper == NULL) { return NULL; }
    else {
        helper->getFreeMemory = getFreeMemory;
        helper->getStackPointerValue = getStackpointerValue;
        return helper;
    }
}
