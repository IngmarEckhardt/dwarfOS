#ifndef UART_HELPER_H
#define UART_HELPER_H

#include <avr/io.h>


void USART_Transmit(unsigned char data);

void USART_TransmitString(char* str);

void USART_ReceiveLine(char* buffer, uint8_t buffer_size);

void sendMsgWithTimestamp(char* message);

#endif //UART_HELPER_H
