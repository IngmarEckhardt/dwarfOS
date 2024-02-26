#include <avr/io.h>
#include <uart_helper.h>
#include <time.h>
#include <avr/interrupt.h>
#include <stdarg.h>
#include <string.h>

uint16_t computeArrayWithStringLengths(va_list args, uint16_t * stringLengthArray);

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

    while (*str && ((*str != '\0'))) {
        usartTransmitChar(*str++);
    }
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

void sendMsgWithTimestamp(int amountOfStrings, ...) {
    va_list args;
    va_start(args, amountOfStrings);

    uint16_t * stringLengthArray = (uint16_t *) (uint16_t volatile) calloc(amountOfStrings, sizeof(uint16_t));
    if (stringLengthArray == NULL) {
        return;
    }
    uint16_t total_length = computeArrayWithStringLengths(args, stringLengthArray);

#ifdef DWARFOS_TIME_H
    uint32_t timeStamp = time(NULL);
    char * volatile localtimeStringpointer = ctime(&timeStamp);
    // Plus two for \r and \n, but not plus 3, \0 isn't necessary, it's included in the TIMESTAMP_LENGTH
    char * volatile concatenated = (char *) malloc((TIMESTAMP_LENGTH+total_length+2) * sizeof(char));
#else
    char * volatile concatenated = (char *) malloc((total_length +3) * sizeof(char));
#endif /*DWARFOS_TIME_H */

    if (concatenated == NULL) {
        free(stringLengthArray);
#ifdef DWARFOS_TIME_H
        free(localtimeStringpointer);
#endif /*DWARFOS_TIME_H */
        return;
    }


    char * volatile dest = concatenated;

#ifdef DWARFOS_TIME_H
    memcpy(dest, localtimeStringpointer, TIMESTAMP_LENGTH-1);
    dest += TIMESTAMP_LENGTH-1;
#endif /*DWARFOS_TIME_H */

    va_start(args, amountOfStrings);
    for (int i = 0; i < amountOfStrings; i++) {
        const char *src = va_arg(args, const char*);
        memcpy(dest, src, stringLengthArray[i]);
        dest += stringLengthArray[i];
    }
    va_end(args);

    (*dest++) = '\r';
    (*dest++) = '\n';
    (*dest) = '\0'; // Null-terminate the concatenated string

    usartTransmitString(concatenated);
    free(concatenated);
#ifdef DWARFOS_TIME_H
    free(localtimeStringpointer);
#endif /*DWARFOS_TIME_H */
}

uint16_t computeArrayWithStringLengths(va_list args, uint16_t * stringLengthArray) {
    uint16_t total_length = 0;
    uint8_t arrayIndex = 0;


    char * str = va_arg(args, char*);
    while (str != NULL) {

        uint16_t volatile len = 0;
        while ((*str) != '\0') {
            len++;
            str++;
        }
        total_length += len;
        stringLengthArray[arrayIndex++] = len;
        str = va_arg(args, char*);
    }
    return total_length;
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