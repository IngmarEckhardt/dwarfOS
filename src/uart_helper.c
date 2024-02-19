#include "uart_helper.h"
#include "time.h"

void USART_Transmit(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));

    // Put data into buffer, sends the data
    UDR0 = data;
}

void USART_TransmitString(const char *str) {
    while (*str) {
        USART_Transmit(*str++);
    }
}

char USART_Receive() {
    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)));

    // Get and return received data from buffer
    return UDR0;
}

void USART_ReceiveLine(char* buffer, uint8_t buffer_size) {
    uint8_t i = 0;
    char received_char;

    while (1) {
        if (i >= buffer_size - 1) {
            buffer[i] = '\0';
            break;
        }
        received_char = USART_Receive();

        // Check if buffer is full or line terminator is received
        if (received_char != '\r') {
            buffer[i++] = received_char;
        } else {
            // Null-terminate the string
            buffer[i] = '\0';
            //consume \n
            received_char = USART_Receive();
            break;
        }
    }
}

void sendMsgWithTimestamp(const char *message) {
    uint32_t timeStamp = time(NULL);
    USART_TransmitString(ctime(&timeStamp));
    USART_TransmitString(" ");
    USART_TransmitString(message);
    USART_TransmitString("\r\n");
}