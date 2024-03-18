#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#ifdef __AVR_HAVE_ELPM__
#include <avr/pgmspace.h>
#endif

//DwarfOS
#include "setup.h"
#include "ascii_helper.h"
#include "mcu_clock.h"
#include "uart_helper.h"
#include "heap_management_helper.h"
#include "time.h"
#include "flash_helper.h"

//the clock has states, instance it one time and hold a reference somewhere if you use it
McuClock * mcuClock;
// we want to call the test function only one time per second
uint8_t lastTime;

// we can use the CPU Frequency as a clock with very less accuracy just for logging purpose
#ifndef DWARFOS_WATCH_QUARTZ
volatile uint8_t adjustCounter = 0;
const uint8_t adjustValue = ADJUST_TO_SECOND_VALUE;
#endif /* DWARFOS_WATCH_QUARTZ */

void adjustTo1Sec(void);
//
void testOSMethod(void);


int main(void) {

    setupMcu(&mcuClock);
    //global Interrupts on, could be moved to setup if you don't have other tasks before entering the loop
    sei();

    while (1) {

        sleep_mode();

#ifndef DWARFOS_WATCH_QUARTZ
        adjustTo1Sec();
#endif /* DWARFOS_WATCH_QUARTZ */

        // for the example its enough to show you something every second, else we go immediately back to sleep
        if ((uint8_t) time(NULL) != lastTime) {

            lastTime = time(NULL);
            testOSMethod();
        }
    }
}




// This overflow interrupt is connected to counter 2, but counter 2 runs on the system clock
// for real time functionality you need a watch quartz at the TOSC1 and TOSC2 pins and change the setup to use it
ISR(TIMER2_OVF_vect) {
#ifdef DWARFOS_WATCH_QUARTZ
    mcuClock->incrementClockOneSec();
}
#else
    adjustCounter++;
}

// counter overflow in this setup is connected to system clock
// 16Mhz / (pre scaling 1024 x overflow interrupt 256 x 61) = ca 1.0001hz, good enough for logging
void adjustTo1Sec(void) {
    if (adjustCounter == adjustValue) {
        mcuClock->incrementClockOneSec();
        adjustCounter = 0;
    }
}
#endif /* DWARFOS_WATCH_QUARTZ */



// example placement of string in Progmem
#define MEMORY_STRING_LENGTH 26
const __attribute__((__progmem__)) char memoryStringOnFlash[MEMORY_STRING_LENGTH] = ": free Memory is (byte): ";

void testOSMethod(void) {

    HeapManagementHelper * heapHelper = dOS_initHeapManagementHelper();
    // result 2003 byte on AtMega328p, overhead incl heapHelper only 45byte
    if (heapHelper) {
        int16_t memoryAmount = heapHelper->getFreeMemory();
        char * memoryAmountString = calloc(5, sizeof(char));

        AsciiHelper * asciiHelper = dOS_initAsciiHelper();
        // just checking as reflex if allocation failed
        if (asciiHelper && memoryAmountString) {
            asciiHelper->integerToAscii(memoryAmountString, memoryAmount, 4, 0);
        }
        free(asciiHelper);
        char * memoryString = malloc(MEMORY_STRING_LENGTH + 1);

        if (memoryString && memoryAmountString) {
            FlashHelper * flashHelper = dOS_initFlashHelper();
#ifdef __AVR_HAVE_ELPM__
            if(flashHelper) {
            flashHelper->loadFarStringFromFlash(memoryString, pgm_get_far_address(memoryStringOnFlash));
            }
#else
            if (flashHelper) {
                flashHelper->loadNearStringFromFlash(memoryString, memoryStringOnFlash);
            }
#endif
            free(flashHelper);

            UartHelper * uartHelper = dOS_initUartHelper();
            // you can find easily memory leaks if you make such a check at several places in your code
            if (uartHelper) {
                uartHelper->sendMsgWithTimestamp(2, (char * []) {memoryString, memoryAmountString});
                //need a small delay until we go to sleep, that the receiver can read the end of our message,
                // otherwise only \r is read not \n
                uartHelper->usartTransmitChar('\0');
            }
            free(uartHelper);

        }
        free(memoryAmountString);
        free(memoryString);
    }
    free(heapHelper);
}

