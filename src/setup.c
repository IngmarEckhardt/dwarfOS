#include "setup.h"

void setupMcu() {
    // I/O
    setSystemClock(INIT_TIME);
    setCPUParamRegister();
}

void setCPUParamRegister() {//Energy saving
    // USART INIT

    // Set baud rate
    UBRR0H = (unsigned char)(UBRR_VAL >> 8);
    UBRR0L = (unsigned char)UBRR_VAL;

    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Energy Saving

    //turn off Analog Comparator
    //ACSR |= (1 << ACD);

    //turning off other modules except timer 2 and USART
    PRR |= (1 << PRTWI) | (1 << PRTIM1) | (1 << PRTIM0) | (1 << PRSPI) | (1 << PRADC);

    //turn off digital input buffers for analog channels
    //DIDR1 |= (1 << AIN1D)  | (1 <<  AIN0D);

    //Counter
    //Using asynchronous timer with low frequency crystal
    ASSR |= (1 << AS2);

    //Prescaler 128 (256x128 = 32768 until overflow interrupt))
    TCCR2B |= (1 << CS22) | (1 << CS20);

    //Overflow-Counter 2 Interrupt on -> overflow every 1sec precisly with 32kHz watch quartz
    TIMSK2 |= (1 << TOIE2);

    //External Standby Sleep Mode, Counter 2 is the only clock that stays active
    SMCR |= (1 << SM2) | (1 << SM1) | (1 << SM0);
    //global interrupts on
    sei();
}