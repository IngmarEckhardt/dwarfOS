#ifndef DWARFOS_PGM_TEXTFILE_GENERATOR_H
#define DWARFOS_PGM_TEXTFILE_GENERATOR_H

#include <stdint.h>

// Provide a const array with non-const strings, specify the number of strings and the maximum number of strings for each individual array, as well as the number of borders you provide.
// This function will create arrays of const structures with a uint8_t index information, indicating the index where the string originally existed in the array, and append the string as a const char
// array.
// The arrays must have constant-sized placeholders for the individual sized strings to fit into PROGMEM.
// Strings that do not fit into your array sizes will be stored as single strings in PROGMEM.
// The usage of this converter can be observed in the colossal-cave-adventure repository (located in the /test folder, with the output included in the project).
void convertTextArrayToProgMemTextFiles(const char ** textsArray, uint16_t amount, const uint16_t bordersArray[],
                                        uint8_t amountBorders, char * prefix);

#endif //DWARFOS_PGM_TEXTFILE_GENERATOR_H
