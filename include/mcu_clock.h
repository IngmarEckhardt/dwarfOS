#ifndef MCU_CLOCK_H
#define MCU_CLOCK_H

#include <stdint.h>


typedef struct {
    uint32_t volatile systemClock;

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

volatile McuClock * dOS_initMcuClock(uint32_t initTime);


#endif /* MCU_CLOCK_H */