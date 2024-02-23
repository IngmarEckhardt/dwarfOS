#ifndef CAT_FOUNTAIN_UART_HELPER_H
#define CAT_FOUNTAIN_UART_HELPER_H

#include "time.h"


void usartTransmit(unsigned char data);

void usartTransmitString(char* str);

void usartReceiveLine(char* buffer, uint8_t bufferSize);

void sendMsgWithTimestamp(char* message);

#endif //CAT_FOUNTAIN_UART_HELPER_H
