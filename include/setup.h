#ifndef DWARFOS_SETUP_H
#define DWARFOS_SETUP_H


#include <mcu_clock.h>
#include <string_repository.h>
#include <string_storage.h>
#include <uart_helper.h>

#ifndef INIT_TIME
// Default initial time: 2024-02-18 19:50:00 (CET)
#define INIT_TIME ((uint32_t) 761601000)
#endif
#ifndef F_CPU
// Default CPU frequency: 16 MHz
#define F_CPU 16000000UL
#endif

#ifndef BAUD
// Default baud rate: 28800 bps
#define BAUD 28800UL
#endif

// Calculate UBRR value for given baud rate and CPU frequency
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)

/**
 * @brief Initializes the MCU setup.
 *
 * This function sets up the microcontroller unit (MCU) by initializing various components
 * such as the clock, string repository, UART helper, etc.
 *
 * To configure the MCU setup, call this function once in your program, typically at the beginning.
 *
 * @param mcuClockPointer A pointer to a pointer to the McuClock structure.
 * @param stringRepositoryPointer A pointer to a pointer to the StringRepository structure.
 * @param uartHelperPointer A pointer to a pointer to the UartHelper structure.
 */
void setupMcu(McuClock ** mcuClockPointer, StringRepository ** stringRepositoryPointer, UartHelper ** uartHelperPointer);

#endif //DWARFOS_SETUP_H
