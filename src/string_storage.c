#include <string_storage.h>
#include <stdlib.h>


char* loadStringFromFlash(const char* PROGMEM flashString) {

    char* result;
    result = (char*) malloc((strlen_P(flashString) + 1) * sizeof(char));
	if (result == NULL) {
		return NULL;
	}
	strcpy_P(result, flashString);
	return result;
}

const char initMsgOnFlash[] PROGMEM = " setup complete.";
LazyLoadingString initMsg = {.flashString = initMsgOnFlash, .pointerToString = NULL};