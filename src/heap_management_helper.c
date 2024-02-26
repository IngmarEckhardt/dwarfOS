#include <heap_management_helper.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

int16_t getFreeMemory(void) {
	return __brkval ? ((int16_t) SP) - ((int16_t) __brkval) : ((int16_t) SP) - ((int16_t) &__malloc_heap_start);
}

HeapManagementHelper * dOS_initHeapManagementHelper(void) {
    HeapManagementHelper * helper = malloc(sizeof(HeapManagementHelper));
    if (helper == NULL) {
        return NULL;
    } else {
        helper->getFreeMemory = getFreeMemory;
        return helper;
    }
}