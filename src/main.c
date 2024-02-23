#include <avr/sleep.h>
#include <avr/interrupt.h>

//DwarfOS
#include <setup.h>
#include <mcu_clock.h>

int main(void) {

    setupMcu();

    sei();
    while (1) {
        sleep_mode();
    }
}

// This overflow interrupt is connected to counter 2, but counter 2 runs on the system clock
// for real time functionality you need a watch quartz at the TOSC1 and TOSC2 pins and change the setup to use it
ISR(TIMER2_OVF_vect) {
    incrementClockOneSec();
}