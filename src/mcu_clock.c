#include <avr/interrupt.h>
#include <mcu_clock.h>
#include <stdlib.h>

McuClock * mcuClock;

uint32_t getSystemClock(void) {
    uint32_t ret;
    uint8_t sreg;

    // Disable interrupts and save the previous state
    sreg = SREG;
    cli();

    // Read the value of systemClock
    ret = mcuClock->systemClock;

    // Restore the previous state (enable interrupts if they were enabled before)
    SREG = sreg;

    // Return the value
    return ret;
}

void setSystemClock(uint32_t timestamp) {
    uint8_t sreg;
    // Disable interrupts and save the previous state
    sreg = SREG;
    cli();

    mcuClock->systemClock = timestamp;

    // Restore the previous state (enable interrupts if they were enabled before)
    SREG = sreg;
}

void incrementClockOneSec(void) {
    uint8_t sreg;
    sreg = SREG;
    cli();
    mcuClock->systemClock++;
    SREG = sreg;
}

McuClock * dOS_initMcuClock(uint32_t initTime) {
    mcuClock = malloc(sizeof(McuClock));
    if (mcuClock == NULL) {
        return NULL;
    } else {
        mcuClock->systemClock = 0;
        mcuClock->setSystemClock = setSystemClock;
        mcuClock->getSystemClock = getSystemClock;
        mcuClock->incrementClockOneSec = incrementClockOneSec;
        mcuClock->setSystemClock(initTime);
        return mcuClock;
    }
}