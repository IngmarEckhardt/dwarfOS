#include <avr/interrupt.h>
#include <mcu_clock.h>

volatile uint32_t systemClock;

uint32_t getSystemClock(void) {
	uint32_t ret;
	uint8_t sreg;

	// Disable interrupts and save the previous state
	sreg = SREG;
	cli();

	// Read the value of systemClock
	ret = systemClock;

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

	systemClock = timestamp;

	// Restore the previous state (enable interrupts if they were enabled before)
	SREG = sreg;
}

void incrementClockOneSec(void) {
	uint8_t sreg;
	sreg = SREG;
	cli();
	systemClock++;
	SREG = sreg;
}