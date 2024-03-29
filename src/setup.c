#include <dwarf-os/setup.h>
#include <avr/io.h>
#include <dwarf-os/time.h>
#include <dwarf-os/uart_helper.h>
#include <dwarf-os/flash_helper.h>

void setCpuParamRegister(void);

void loadInitStringAndSendInitMsg(UartHelper * uartHelper);


// ToDo could get options for setup (such as .withUSART = 1,) as a Interface in the function parameters

void setupMcu(McuClock ** pointerToGlobalMcuClockPointer) {

    setCpuParamRegister();

    // derefencering the pointer to the global pointer -> setting the global pointers
    *pointerToGlobalMcuClockPointer = dOS_initMcuClock(INIT_TIME);
    setMcuClockCallback((*pointerToGlobalMcuClockPointer)->getSystemClock);

    UartHelper * uartHelper = dOS_initUartHelper();
    loadInitStringAndSendInitMsg(uartHelper);
    free(uartHelper);
}

void loadInitStringAndSendInitMsg(UartHelper * uartHelper) {
    if (uartHelper == NULL) { return; }

    FlashHelper * flashHelper = dOS_initFlashHelper(2);
    if (flashHelper == NULL) { return; }

    char * initmsg = flashHelper->getOrPutDosMessage(IDENT_STRING, 1, flashHelper);
    uartHelper->sendMsgWithTimestamp(1, (char * []) {initmsg});
    //make sure that the receiver read our char, with a small delay, before a user sends us to sleep mode
    uartHelper->usartTransmitChar('\0',  NULL);
    free(initmsg);
    free(flashHelper);
}

void setCpuParamRegister(void) {

    // USART INIT
    // Set baud rate
    UBRR0H = (uint8_t) (UBRR_VAL >> 8);
    UBRR0L = (uint8_t) UBRR_VAL;
    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Energy Saving

    //turn off Analog Comparator
    ACSR |= (1 << ACD);
#ifdef __AVR_ATmega328P__
    //turning off other modules except timer 2 and USART
    PRR |= (1 << PRTWI) | (1 << PRTIM1) | (1 << PRTIM0) | (1 << PRSPI) | (1 << PRADC);
#elif __AVR_ATmega2560__
    PRR0 |= (1 << PRTWI) | (1 << PRTIM1) | (1 << PRTIM0) | (1 << PRSPI) | (1 << PRADC);
#endif

    //turn off digital input buffers for analog channels
    DIDR1 |= (1 << AIN1D) | (1 << AIN0D);

    //Counter

#ifdef DWARFOS_WATCH_QUARTZ
    //Using asynchronous timer with low frequency crystal
    ASSR |= (1 << AS2);

    //Pre scaling 128 (256x128 = 32768 until overflow interrupt)
    TCCR2B |= (1 << CS22) | (1 << CS20);
#else
    // Prescaler 1024 for using the system clock
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
#endif /* DWARFOS_WATCH_QUARTZ  */

    //Overflow-Counter 2 Interrupt on -> overflow every 1sec precisely with 32kHz watch quartz and pre scaling 128
    //ca 1/61hz overflow with system clock and pre scaling 1024
    TIMSK2 |= (1 << TOIE2);
    //External Standby Sleep Mode, Counter 2 is the only clock that stays active
    SMCR |= (1 << SM2) | (1 << SM1) | (1 << SM0);
}