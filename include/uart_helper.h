#ifndef DWARFOS_UART_HELPER_H
#define DWARFOS_UART_HELPER_H

#include <stdint.h>

void usartTransmitChar(unsigned char byte);

void usartTransmitString(char * str);

void usartReceiveLine(char * buffer, uint8_t bufferSize);

void sendMsgWithTimestamp(int amountOfStrings, ...);

#endif /* DWARFOS_UART_HELPER_H */
