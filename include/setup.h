#ifndef DWARFOS_SETUP_H
#define DWARFOS_SETUP_H

#include <mcu_clock.h>

#ifndef INIT_TIME
// Default initial time: 2024-02-18 19:50:00 (CET)
#define INIT_TIME 761601000UL
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

#define ADJUST_TO_SECOND_VALUE (uint8_t)(F_CPU / ( 1024UL * 256))

/**
 * @brief Initializes the MCU setup.
 *
 * This function sets up the microcontroller unit (MCU) by initializing various components such as the clock,
 * string repository, UART helper, etc. Change this function to your desired behaviour. Load only the modules you need.
 * A minimal setup would be just setting the parameter registers, dont using lazy strings and instancing and freeing
 * the other modules fine granular only when needed.
 *
 * To configure the MCU setup, call this function once in your program, typically at the beginning.
 *
 * @param pointerToGlobalMcuClockPointer A pointer to a pointer to the McuClock structure.
 * @param pointerToGlobalStringRepoPointer A pointer to a pointer to the StringRepository structure.
 * @param pointerToGlobalUartHelperPointer A pointer to a pointer to the UartHelper structure.
 */
void setupMcu(McuClock ** pointerToGlobalMcuClockPointer);

#endif //DWARFOS_SETUP_H
