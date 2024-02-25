#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/io.h>

//DwarfOS
#include <setup.h>
#include <mcu_clock.h>
#include <uart_helper.h>
#include <ascii_helper.h>
#include <string_repository.h>
#include "time.h"

volatile uint8_t adJust16MhzToSecond = 0;
int16_t memory;
 char memoryArray[5];
 volatile uint8_t sended = 0;

 char timeStampArray[12];

int main(void) {
	memoryArray[4] = 0x00;
	timeStampArray[11] = 0x00;

	setupMcu();
	memory = getFreeMemory();
	sendMsgWithTimestamp("free Memory is");
	if (memory < 0) {
		memory = -(memory);
		sendMsgWithTimestamp("minus");
	}
	integerToAscii(memoryArray, memory, 4, 0);
	sendMsgWithTimestamp(memoryArray);
	sei();
	while (1) {
		
		//sleep_mode();
		if(!(time(NULL)%5)){
			if(!sended) {
						sendMsgWithTimestamp("sended was");
						integerToAscii(memoryArray,sended,1,0);
						sendMsgWithTimestamp(memoryArray);
				
						memory = getFreeMemory();
						sendMsgWithTimestamp("free Memory is");
						if (memory < 0) {
							memory = -(memory);
							sendMsgWithTimestamp("minus");
						}
						integerToAscii(memoryArray, memory, 4, 0);
						sendMsgWithTimestamp(memoryArray);
						memoryArray[0] = 	memoryArray[1] = 	memoryArray[2] = 	memoryArray[3] = 	memoryArray[4] = 0;
						sendMsgWithTimestamp("sended was");
						integerToAscii(memoryArray,sended,1,0);
						sendMsgWithTimestamp(memoryArray);
						
						
						
							sendMsgWithTimestamp("pin D was");
							DDRD &= ~(PIND2);
							uint8_t result = PIND & (1 << PIND2);
							integerToAscii(memoryArray,result,1,0);
							sendMsgWithTimestamp(memoryArray);
						sended = 1;
						
			}
			
			

		} else {
			sended = 0;
		}
		
	}
}

// This overflow interrupt is connected to counter 2, but counter 2 runs on the system clock
// for real time functionality you need a watch quartz at the TOSC1 and TOSC2 pins and change the setup to use it
ISR(TIMER2_OVF_vect) {
	    adJust16MhzToSecond++;
	    if (adJust16MhzToSecond == 61) {
		    incrementClockOneSec();
		    adJust16MhzToSecond = 0;
	    }
}