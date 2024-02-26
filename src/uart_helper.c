#include <avr/io.h>
#include <uart_helper.h>
#include <time.h>
#include <avr/interrupt.h>
#include "ascii_helper.h"


void usartTransmitChar(uint8_t byte) {
    uint8_t sreg;

    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));
    // Disable interrupts and save the previous state
    sreg = SREG;
    cli();

    // Put byte into buffer, sends the byte
    UDR0 = byte;
    while (!(UCSR0A & (1 << UDRE0)));
    // Restore the previous state (enable interrupts if they were enabled before)
    SREG = sreg;
}

void usartTransmitString(char * volatile str) {
    if (str == NULL) {
        return;
    }

    while (*str != '\0') {
        usartTransmitChar(*str++);
    };


    usartTransmitChar('\0');
}

char usartReceive(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;

}

void usartReceiveLine(char * buffer, uint8_t bufferSize) {
    uint8_t i = 0;
    char receivedChar;

    while (1) {
        if (i >= bufferSize - 1) {
            buffer[i] = 0x00;
            break;
        }
        receivedChar = usartReceive();


        if (receivedChar != 0x0d) {
            buffer[i++] = receivedChar;
        } else {

            buffer[i] = '\0';

            receivedChar = usartReceive();
            break;
        }
    }
}

void sendMsgWithTimestamp(int amountOfStrings, char * strings[]) {

#ifdef DWARFOS_TIME_H
    uint32_t timeStamp = time(NULL);
    char * volatile localtimeStringpointer = ctime(&timeStamp);
#endif /*DWARFOS_TIME_H */

    // Create a new array to hold the sorted pointers
    char ** sortedStrings = (char **) malloc((amountOfStrings + 1) * sizeof(char *));
    if (sortedStrings == NULL) {
#ifdef DWARFOS_TIME_H
        free(localtimeStringpointer);
#endif /*DWARFOS_TIME_H */
        return;
    }

    // Set the timestamp at the first position
#ifdef DWARFOS_TIME_H
    sortedStrings[0] = localtimeStringpointer;
#else
    // If time.h is not available, you may want to handle this case accordingly
    sortedStrings[0] = '\0'; // Placeholder for timestamp
#endif /* DWARFOS_TIME_H */

    // Copy the original strings pointers to the sorted array
    for (int i = 0; i < amountOfStrings; i++) {
        sortedStrings[i + 1] = strings[i];
    }


    AsciiHelper * asciiHelper = dOS_initAsciiHelper();
    char * concatenated = asciiHelper->concatStrings(amountOfStrings + 1, sortedStrings);
    if (concatenated == NULL) {
#ifdef DWARFOS_TIME_H
        free(localtimeStringpointer);
#endif /*DWARFOS_TIME_H */
        free(sortedStrings);
        return;
    }

    usartTransmitString(concatenated);
    free(sortedStrings);
    free(concatenated);


#ifdef DWARFOS_TIME_H
    free(localtimeStringpointer);
#endif /*DWARFOS_TIME_H */
}


UartHelper * dOS_initUartHelper(void) {
    UartHelper * helper = malloc(sizeof(UartHelper));
    if (helper == NULL) {
        return NULL;
    } else {
        helper->sendMsgWithTimestamp = sendMsgWithTimestamp;
        helper->usartTransmitChar = usartTransmitChar;
        helper->usartTransmitString = usartTransmitString;
        helper->usartReceiveLine = usartReceiveLine;
        return helper;
    }
}