#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/io.h>

//DwarfOS
#include <setup.h>
#include <mcu_clock.h>
#include <uart_helper.h>
#include <string_repository.h>
#include <string_storage.h>
#include "time.h"

McuClock * mcuClock;
StringRepository * stringRepository;

volatile uint8_t adJust16MhzToSecond = 0;
uint32_t lastTime;

int main(void) {
	
	setupMcu(mcuClock, stringRepository);
    sei();

	while (1) {
        sleep_mode();
		if (time(NULL) != lastTime) {
			sendMsgWithTimestamp(1,stringRepository->getString(&initMsg));
			lastTime = time(NULL);
		}	
	}
}

// This overflow interrupt is connected to counter 2, but counter 2 runs on the system clock
// for real time functionality you need a watch quartz at the TOSC1 and TOSC2 pins and change the setup to use it
ISR(TIMER2_OVF_vect) {
	    adJust16MhzToSecond++;
	    if (adJust16MhzToSecond == 61) {
		    mcuClock->incrementClockOneSec();
		    adJust16MhzToSecond = 0;
	    }
}