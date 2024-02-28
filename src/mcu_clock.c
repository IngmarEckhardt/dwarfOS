#include <avr/interrupt.h>
#include <mcu_clock.h>
#include <stdlib.h>

McuClock * mcuClock;

uint32_t getSystemClock(void) {
    uint32_t returnValue;
    uint8_t interruptStatusRegister;

    // Disable interrupts and save the previous state, because systemClock has 4 bytes and need 4 clock cycles
    // for read and write operations that shouldn't be interrupted
    interruptStatusRegister = SREG;
    cli();

    returnValue = mcuClock->systemClock;

    // Restore the previous state (enable interrupts if they were enabled before)
    SREG = interruptStatusRegister;

    return returnValue;
}

void setSystemClock(uint32_t timestamp) {
    uint8_t interruptStatusRegister;

    interruptStatusRegister = SREG;
    cli();

    mcuClock->systemClock = timestamp;

    SREG = interruptStatusRegister;
}

void incrementClockOneSec(void) {
    uint8_t interruptStatusRegister;
    interruptStatusRegister = SREG;
    cli();

    mcuClock->systemClock++;

    SREG = interruptStatusRegister;
}

McuClock * dOS_initMcuClock(uint32_t initTime) {
    mcuClock = malloc(sizeof(McuClock));
    if (mcuClock == NULL) { return NULL; }
    else {
        mcuClock->setSystemClock = setSystemClock;
        mcuClock->getSystemClock = getSystemClock;
        mcuClock->incrementClockOneSec = incrementClockOneSec;
        mcuClock->setSystemClock(initTime);
        return mcuClock;
    }
}