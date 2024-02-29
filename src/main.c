#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/io.h>

//DwarfOS
#include <setup.h>
#include <ascii_helper.h>
#include <mcu_clock.h>
#include <uart_helper.h>
#include <heap_management_helper.h>
#include <time.h>

//the clock has states, instance it one time and hold a reference somewhere if you use it
McuClock * mcuClock;

#ifdef DWARFOS_WATCH_QUARTZ
#else
const uint8_t adjustValue = ADJUST_TO_SECOND_VALUE;
volatile uint8_t adjustCounter = 0;
#endif /* DWARFOS_WATCH_QUARTZ */

// just for the example
uint8_t lastTime;

void adjustTo1Sec(void);

void testOSMethod(void);

int main(void) {

    setupMcu(&mcuClock);

    //global Interrupts on, could be moved to setup if you don't have other tasks before entering the loop
    sei();
    while (1) {

        sleep_mode();

#ifdef DWARFOS_WATCH_QUARTZ
#else
        adjustTo1Sec();
#endif /* DWARFOS_WATCH_QUARTZ */


        // for the example its enough to show you something every second, else we go immediately back to sleep
        if ((uint8_t)time(NULL) != lastTime) {
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
#else
    adjustCounter++;
#endif /* DWARFOS_WATCH_QUARTZ */
}

#ifdef DWARFOS_WATCH_QUARTZ
#else
// counter overflow in this setup is connected to system clock
// 16Mhz / (pre scaling 1024 x overflow interrupt 256 x 61) = ca 1.0001hz, good enough for logging
void adjustTo1Sec(void) {
    if (adjustCounter == adjustValue) {
        mcuClock->incrementClockOneSec();
        adjustCounter = 0;
    }
}
#endif /* DWARFOS_WATCH_QUARTZ */


void testOSMethod(void) {
    char memoryStringArray[5];
    memoryStringArray[4] = '\0';

    HeapManagementHelper * heapHelper = dOS_initHeapManagementHelper();
    // result 1934 byte
    uint16_t memoryAmount = heapHelper->getFreeMemory();
    free(heapHelper);

    AsciiHelper * asciiHelper = dOS_initAsciiHelper();
    asciiHelper->integerToAscii(memoryStringArray, memoryAmount, 4, 0);
    free(asciiHelper);

    UartHelper * uartHelper = dOS_initUartHelper();
    // you can find easily memory leaks if you make such a check at several places in your code
    uartHelper->sendMsgWithTimestamp(3, (char * []) {"free Memory is: ", memoryStringArray, "byte"});

    //need a small delay until we go to sleep, that the receiver can read the end of our message,
    // otherwise only \r is read not \n
    uartHelper->usartTransmitChar('\0');
    free(uartHelper);
}