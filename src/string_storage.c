#include <string_storage.h>
#include <stdlib.h>


char* loadStringFromFlash(const char* PROGMEM flashString) {
	uint16_t length = strlen_P(flashString) + 1;
	char* result;
	result = (char*) calloc(length, 1);
	if (result == NULL) {
		return NULL;
	}
	strcpy_P(result, flashString);
	return result;
}

const char initMsgOnFlash[] PROGMEM = " setup complete ";
LazyLoadingString initMsg = {.flashString = initMsgOnFlash, .pointerToString = NULL};