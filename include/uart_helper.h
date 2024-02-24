#ifndef DWARFOS_UART_HELPER_H
#define DWARFOS_UART_HELPER_H
#include <stdint.h>

void usartTransmit(unsigned char data);

void usartTransmitString(char* str);

void usartReceiveLine(char* buffer, uint8_t bufferSize);

void sendMsgWithTimestamp(char* message);

#endif /* DWARFOS_UART_HELPER_H */
