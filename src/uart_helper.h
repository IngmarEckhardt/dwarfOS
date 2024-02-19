#ifndef UART_HELPER_H
#define UART_HELPER_H

#define MAX_BUFFER_SIZE 81 //80 signs strings and a \0 as termination

#include <avr/io.h>

void USART_Transmit(unsigned char data);
void USART_TransmitString(const char *str);
void USART_ReceiveLine(char *buffer, uint8_t buffer_size);
void sendMsgWithTimestamp(const char *message);

#endif //UART_HELPER_H
