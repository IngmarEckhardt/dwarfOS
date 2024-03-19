/**
 * @file stdio.h
 * @brief Standard input/output library for DwarfOS.
 */

#ifndef DWARFOS_STDIO_H
#define DWARFOS_STDIO_H
#include <stdint.h>

/**
 * @brief Outputs a string located in program memory to the standard output.
 *
 * This function reads a null-terminated string from the program memory,
 * located at the address specified by `farPointerToString`, and writes it
 * to the standard output. The string must be located in program memory.
 *
 * If the standard output is not writeable, or if an error occurs while
 * writing, the function will return EOF. Otherwise, it will return 0.
 *
 * Note: This function is only available on AVR microcontrollers that support
 * the ELPM instruction (extended load program memory).
 *
 * @param farPointerToString A far pointer to the string in program memory.
 * @return 0 on success, or EOF on error.
 */
int16_t puts_PF(uint32_t farPointerToString);

#endif //DWARFOS_STDIO_H
