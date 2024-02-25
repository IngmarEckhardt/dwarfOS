#include <ascii_helper.h>

void integerToAscii(volatile char* result, uint32_t num, uint8_t size, uint8_t position) {
    uint32_t divisor = 1;
    for (int i = 0; i < size - 1; ++i) {
        divisor *= 10;
    }

    for (uint8_t i = position; i < position + size; ++i) {
        uint8_t digit = num / divisor;
        result[i] = digit + '0';
        num %= divisor;
        divisor /= 10;
    }
}