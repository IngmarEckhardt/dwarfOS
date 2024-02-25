#include <string_repository.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

int16_t getFreeMemory(void) {
	return __brkval ? ((int16_t) SP) - ((int16_t) __brkval) : ((int16_t) SP) - ((int16_t) &__malloc_heap_start);
}
