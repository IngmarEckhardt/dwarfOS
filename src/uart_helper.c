#include <avr/io.h>
#include <uart_helper.h>
#include <time.h>

void usartTransmit(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));

    // Put data into buffer, sends the data
    UDR0 = data;
}

void usartTransmitString(char* str) {

    while (*str) {
        usartTransmit(*str++);
    }
}

char usartReceive(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;

}

void usartReceiveLine(char* buffer, uint8_t bufferSize) {
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

            buffer[i] = 0x00;

            receivedChar = usartReceive();
            break;
        }
    }
}

void sendMsgWithTimestamp(char* message) {

#ifdef DWARFOS_TIME_H
    uint32_t timeStamp = time(NULL);
    char* localtimeStringpointer = ctime(&timeStamp);
    usartTransmitString(localtimeStringpointer);
    usartTransmit(0x20);
    free(localtimeStringpointer);
#endif /*DWARFOS_TIME_H */
    usartTransmitString(message);
    usartTransmit(0x0d);
    usartTransmit(0x0a);
}