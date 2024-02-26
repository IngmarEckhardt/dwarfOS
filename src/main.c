#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/io.h>

//DwarfOS
#include <setup.h>
#include <ascii_helper.h>
#include <mcu_clock.h>
#include <uart_helper.h>
#include <string_repository.h>
#include <string_storage.h>
#include <heap_management_helper.h>
#include "time.h"

McuClock * mcuClock;
StringRepository * stringRepository;
UartHelper * uartHelper;

volatile uint8_t adJust16MhzToSecond = 0;
uint32_t lastTime;


void adjustTo1Sec(void);

void testOSMethod(void);

int main(void) {

    setupMcu(&mcuClock, &stringRepository, &uartHelper);
    sei();

    while (1) {
        sleep_mode();
        adjustTo1Sec();

        if (time(NULL) != lastTime) {
            lastTime = time(NULL);
            testOSMethod();

        }
    }
}

// This overflow interrupt is connected to counter 2, but counter 2 runs on the system clock
// for real time functionality you need a watch quartz at the TOSC1 and TOSC2 pins and change the setup to use it
ISR(TIMER2_OVF_vect) {
    adJust16MhzToSecond++;

}

void adjustTo1Sec(void) {
    if (adJust16MhzToSecond == 61) {
        mcuClock->incrementClockOneSec();
        adJust16MhzToSecond = 0;
    }
}


void testOSMethod(void) {
	char memoryStringArray[5];
	memoryStringArray[4] = '\0';
	
	HeapManagementHelper * heapHelper = dOS_initHeapManagementHelper();
	uint16_t memoryAmount = heapHelper->getFreeMemory();
	free(heapHelper);
	
	AsciiHelper * asciiHelper = dOS_initAsciiHelper();
	asciiHelper->integerToAscii(memoryStringArray, memoryAmount,4,0);
	free(asciiHelper);
	
	uartHelper->sendMsgWithTimestamp(3, (char * []) {"free Memory is: ",memoryStringArray, " byte"});
	
    //we need a small delay until we go to sleep, that the receiver can read our message
    uartHelper->usartTransmitChar('\0');
}