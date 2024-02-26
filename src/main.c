#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/io.h>

//DwarfOS
#include <setup.h>
#include <mcu_clock.h>
#include <uart_helper.h>
#include <string_repository.h>
#include <string_storage.h>
#include <ascii_helper.h>
#include "time.h"

McuClock * mcuClock;
StringRepository * stringRepository;
StringStorage * stringStorage;
UartHelper * uartHelper;

volatile uint8_t adJust16MhzToSecond = 0;
uint32_t lastTime;

int main(void) {
	
	setupMcu(mcuClock, stringRepository, stringStorage, uartHelper);
    sei();
	AsciiHelper * asciiHelper = dOS_initAsciiHelper();
	char timeArray[10];
	timeArray[9]= '\0';

	while (1) {
        sleep_mode();
		 // if (adJust16MhzToSecond == 61) {
			  //mcuClock->incrementClockOneSec();
			  //adJust16MhzToSecond = 0;
		//  }
		if (time(NULL) != lastTime) {
			//lastTime = time(NULL);
			//asciiHelper->integerToAscii(timeArray, lastTime, 8,0);
			//uartHelper->sendMsgWithTimestamp(4,stringRepository->getString(&stringStorage->initMsg, stringStorage),"time is",timeArray);
			uartHelper->sendMsgWithTimestamp(1, ctime(NULL));
			
		}	
	}
}

// This overflow interrupt is connected to counter 2, but counter 2 runs on the system clock
// for real time functionality you need a watch quartz at the TOSC1 and TOSC2 pins and change the setup to use it
ISR(TIMER2_OVF_vect) {
	    adJust16MhzToSecond++;
	  
}