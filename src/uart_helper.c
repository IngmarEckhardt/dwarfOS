#include "uart_helper.h"
#include <avr/io.h>
#include "time.h"
#include "ascii_helper.h"

// Transmit a single byte via USART
void usartTransmitChar(uint8_t byte) {

    // wait for free transmit Buffer
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = byte;
    while (!(UCSR0A & (1 << UDRE0)));
}

// Transmit a null-terminated string via USART
void usartTransmitString(char * str) {
    if (str == NULL) { return; }

    while (*str != '\0') { usartTransmitChar(*str++); }
    usartTransmitChar('\r');
    usartTransmitChar('\n');
}

// Receive a single byte via USART
uint8_t usartReceive(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

// Receive a line of characters via USART
void usartReceiveLine(char * buffer, uint8_t bufferSize) {
    if (buffer == NULL) { return; }

    uint8_t index = 0;
    uint8_t receivedChar;

    while (1) {
        // if buffer is full, we have to null terminate the buffer
        if (index >= bufferSize - 1) {
            buffer[index] = '\0';
            break;
        }
        receivedChar = usartReceive();
        // read bytes until we reach \r
        if (receivedChar != 0x0d) { buffer[index++] = receivedChar; }
        else {
            buffer[index] = '\0';
            // consume \n
            usartReceive();
            break;
        }
    }
}

// Send a message with timestamp via USART
void sendMsgWithTimestamp(uint8_t amountOfStrings, char * strings[]) {

#ifdef DWARFOS_TIME_H
    char * localtimeStringpointer = ctime(NULL);
#endif /*DWARFOS_TIME_H */

    // Create a new array to hold the sorted pointers with purpose to insert timestamp at the start of line
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

    for (uint8_t i = 0; i < amountOfStrings; i++) { sortedStrings[i + 1] = strings[i]; }

    AsciiHelper * asciiHelper = dOS_initAsciiHelper();
    char * concatenatedString = asciiHelper->concatStrings(amountOfStrings + 1, sortedStrings);
    free(asciiHelper);

    if (concatenatedString == NULL) {
#ifdef DWARFOS_TIME_H
        free(localtimeStringpointer);
#endif /*DWARFOS_TIME_H */
        free(sortedStrings);
        return;
    }

    usartTransmitString(concatenatedString);
    free(sortedStrings);
    free(concatenatedString);

#ifdef DWARFOS_TIME_H
    free(localtimeStringpointer);
#endif /*DWARFOS_TIME_H */
}

UartHelper * dOS_initUartHelper(void) {
    UartHelper * helper = malloc(sizeof(UartHelper));
    if (helper == NULL) { return NULL; }
    else {
        helper->sendMsgWithTimestamp = sendMsgWithTimestamp;
        helper->usartTransmitChar = usartTransmitChar;
        helper->usartTransmitString = usartTransmitString;
        helper->usartReceiveLine = usartReceiveLine;
        return helper;
    }
}