#include <heap_management_helper.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

extern char * __brkval;

int16_t getFreeMemory(void) {
    // SP is the stack pointer, this calculation ignores fragmented memory inside the area that is managed from malloc
    // but already freed and the space between bss and malloc heap end, if first allocated memory is freed.
    // There is no guarantee that you can use these fragments if they are too small for the next allocated data structure
    return __brkval ? ((int16_t) SP) - ((int16_t) __brkval) : ((int16_t) SP) - ((int16_t) &__malloc_heap_start);
}

HeapManagementHelper * dOS_initHeapManagementHelper(void) {
    HeapManagementHelper * helper = malloc(sizeof(HeapManagementHelper));
    if (helper == NULL) { return NULL; }
    else {
        helper->getFreeMemory = getFreeMemory;
        return helper;
    }
}