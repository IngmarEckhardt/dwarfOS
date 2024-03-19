#include <dwarf-os/mcu_clock.h>
#include <avr/interrupt.h>
#include <stdlib.h>

McuClock * mcuClock;

uint32_t getSystemClock(void) {
    uint32_t returnValue;
    ATOMIC_OPERATION(returnValue = mcuClock->systemClock)
    return returnValue;
}

void setSystemClock(uint32_t timestamp) { ATOMIC_OPERATION(mcuClock->systemClock = timestamp) }

void incrementClockOneSec(void) { ATOMIC_OPERATION(mcuClock->systemClock++) }

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