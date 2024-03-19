#ifndef MCU_CLOCK_H
#define MCU_CLOCK_H

#include <stdint.h>

/**
 * @brief Structure for managing the MCU (Microcontroller Unit) clock.
 *
 * McuClock provides functions for initializing, retrieving, and incrementing the system clock.
 * The system clock represents the current time in seconds.
 */
typedef struct {
    uint32_t volatile systemClock; // Current system clock value

    /**
    * @brief Initializes the system clock with the given timestamp.
    *
    * This function initializes the system clock with the provided timestamp value.
    *
    * @param timestamp The timestamp to set the system clock to.
    */
    void (* setSystemClock)(uint32_t timestamp);

    /**
    * @brief Retrieves the current system clock value.
    *
    * This function retrieves the current value of the system clock.
    *
    * @return The current system clock value.
    */
    uint32_t (* getSystemClock)(void);


    /**
    * @brief Increases the system clock value by one second.
    *
    * This function increases the system clock value by one second.
    * It is typically called in an interrupt routine that is invoked from a counter overflow counting a clock quartz.
    */
    void (* incrementClockOneSec)(void);
} McuClock;
/**
 * @brief Initializes the MCU clock with the specified initial time.
 *
 * This function initializes the MCU clock with the given initial time value.
 * It returns a pointer to the McuClock structure, allowing access to clock management functions.
 *
 * To use the MCU clock, call this initialization function once in your program, typically at the beginning or whenever
 * MCU clock functionality is required.
 *
 * When MCU clock functionality is no longer needed or to conserve memory, you can unload the MCU clock module by freeing
 * the allocated memory using the standard `free()` function.
 *
 * @note Make sure to check if the returned pointer is not NULL before using the functions.
 *
 * @param initTime The initial time value to set the system clock to.
 * @return A pointer to the initialized McuClock structure.
 */
McuClock * dOS_initMcuClock(uint32_t initTime);

#define ATOMIC_OPERATION(code) \
        uint8_t interruptStatusRegister; \
        interruptStatusRegister = SREG; \
        cli(); \
        code; \
        SREG = interruptStatusRegister; \


#endif /* MCU_CLOCK_H */