#ifndef HEAP_MANAGEMENT_HELPER_H_
#define HEAP_MANAGEMENT_HELPER_H_

#include <avr/pgmspace.h>
#include <stdlib.h>

extern char * __brkval;

typedef struct {
    int16_t (* getFreeMemory)(void);
} HeapManagementHelper;

HeapManagementHelper * dOS_initHeapManagementHelper(void);


#endif /* HEAP_MANAGEMENT_HELPER_H_ */