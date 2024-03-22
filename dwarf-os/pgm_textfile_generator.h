#ifndef DWARFOS_PGM_TEXTFILE_GENERATOR_H
#define DWARFOS_PGM_TEXTFILE_GENERATOR_H

#include <stdint.h>

/**
 * @brief Converts an array of strings into program memory text files.
 *
 * This function takes an array of strings and creates arrays of constant structures in program memory.
 * Each structure contains a uint8_t index information, indicating the original index of the string in the array,
 * and a string as a constant character array.
 *
 * The arrays must have constant-sized placeholders for the individual sized strings to fit into PROGMEM.
 * Strings that do not fit into your array sizes will be stored as single strings in PROGMEM.
 *
 * @param arrayWithStrings A constant array with non-constant strings.
 * @param amountOfStrings The number of strings in the array.
 * @param maxSizesOfStringsInPGMArrays An array specifying the maximum number of strings for each individual array.
 * @param desiredAmountPGMArrays The desired number of program memory arrays.
 * @param desiredNamingAllUpperCase A string representing the desired naming in all uppercase with underlines as separators.
 * @param desiredWithIndexArray A flag indicating whether to include index array. If set to 1, index array will be included.
 */
void convertStringsToPGMTextFile(const char ** arrayWithStrings, uint16_t amountOfStrings,
                                 const uint16_t maxSizesOfStringsInPGMArrays[],
                                 uint8_t desiredAmountPGMArrays, char * desiredNamingAllUpperCase,
                                 uint8_t desiredWithIndexArray);

#endif //DWARFOS_PGM_TEXTFILE_GENERATOR_H
