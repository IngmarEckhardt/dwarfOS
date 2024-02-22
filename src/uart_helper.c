#include "uart_helper.h"
#include <avr/io.h>


void USART_Transmit(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));

    // Put data into buffer, sends the data
    UDR0 = data;
}

void USART_TransmitString(char *str) {

	   while (*str) {
		   USART_Transmit(*str++);
	   }
}

char USART_Receive() {
       while (!(UCSR0A & (1 << RXC0)));
    return UDR0;

}

void USART_ReceiveLine(char* buffer, uint8_t buffer_size) {
    uint8_t i = 0;
    char received_char;

    while (1) {
        if (i >= buffer_size - 1) {
            buffer[i] = 0x00;
            break;
        }
        received_char = USART_Receive();


        if (received_char != 0x0d) {
            buffer[i++] = received_char;
        } else {

            buffer[i] = 0x00;

            received_char = USART_Receive();
            break;
        }
    }
}

void sendMsgWithTimestamp(char *message) {

	uint32_t timeStamp = time(NULL);
    char* localtimeStringpointer = ctime(&timeStamp);
    USART_TransmitString(localtimeStringpointer);
    USART_Transmit(0x20);
    USART_TransmitString(message);
	USART_Transmit(0x0d);
	USART_Transmit(0x0a);
    free(localtimeStringpointer);
}