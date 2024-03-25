#include <_stdio.h>
// standard libs
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//Unity
#include <unity.h>
//DwarfOS
#include <dwarf-os/uart_helper.h>
#include <dwarf-os/flash_helper.h>
#include <dwarf-os/stdio.h>
// given data
#include "lorem_ipsum.h"

UartHelper * uartHelper;
uint8_t verbose = 0;

#define BUFFERSIZE (uint8_t) UINT8_MAX
uint32_t farProgMemStringUnderInspektion;
uint16_t farMemStringIndex;
uint8_t buffer_index = 0;
char * stdoutCopyBuffer;

void setUp(void) {}

void tearDown(void) { stdout->put = uartHelper->usartTransmitChar; }


int mockPutSmallBufferCompare(char c, FILE * stream) {
    if (verbose) { uartHelper->usartTransmitChar(c, stream); }
    stdoutCopyBuffer[buffer_index++] = c;
    return 0;
}

int mockPutLoremIpsumCompare(char c, FILE * stream) {
    if (verbose) { uartHelper->usartTransmitChar(c, stream); }
    char expectedChar = pgm_read_byte_far(farProgMemStringUnderInspektion + farMemStringIndex);
    TEST_ASSERT_EQUAL_CHAR(expectedChar, c);
    farMemStringIndex++;
    return 0;
}

void test_puts_PF(void) {
    // given
    stdoutCopyBuffer = calloc(BUFFERSIZE, sizeof(char));
    buffer_index = 0;
    stdout->put = mockPutSmallBufferCompare;
    uint32_t farPointerToString = (uint32_t) PSTR("Test string");
    // when
    puts_PF(farPointerToString);
    stdoutCopyBuffer[buffer_index] = '\0';
    // then
    TEST_ASSERT_EQUAL_STRING("Test string", stdoutCopyBuffer);
    for (uint8_t i = buffer_index; i < BUFFERSIZE; i++) { TEST_ASSERT_EQUAL_CHAR('\0', stdoutCopyBuffer[i]); }
}

void test_puts_PF_empty_string(void) {
    // given
    stdoutCopyBuffer = calloc(BUFFERSIZE, sizeof(char));
    buffer_index = 0;
    stdout->put = mockPutSmallBufferCompare;
    uint32_t farPointerToString = (uint32_t) PSTR("");
    // when
    puts_PF(farPointerToString);
    stdoutCopyBuffer[buffer_index] = '\0';
    // then
    TEST_ASSERT_EQUAL_STRING("", stdoutCopyBuffer);
    for (uint8_t i = buffer_index; i < BUFFERSIZE; i++) { TEST_ASSERT_EQUAL_CHAR('\0', stdoutCopyBuffer[i]); }
}

void test_puts_PF_long_string(void) {
    // given
    stdoutCopyBuffer = calloc(BUFFERSIZE, sizeof(char));
    buffer_index = 0;
    stdout->put = mockPutSmallBufferCompare;
    uint32_t farPointerToString = (uint32_t) PSTR("This is a long test string");
    // when
    puts_PF(farPointerToString);
    // then
    TEST_ASSERT_EQUAL_STRING("This is a long test string", stdoutCopyBuffer);
    for (uint8_t i = buffer_index; i < BUFFERSIZE; i++) { TEST_ASSERT_EQUAL_CHAR('\0', stdoutCopyBuffer[i]); }
}

void test_puts_PF_loremIpsum(void) {
    //given
    farProgMemStringUnderInspektion = addressOf(loremIpsum8);
    stdout->put = mockPutLoremIpsumCompare; // here are the asserts
    farMemStringIndex = 0;
    // when
    puts_PF(farProgMemStringUnderInspektion);
}

void runPutsPFTests(uint8_t verboseMode) {
    verbose = verboseMode;
    UNITY_BEGIN();
    RUN_TEST(test_puts_PF);
    RUN_TEST(test_puts_PF_empty_string);
    RUN_TEST(test_puts_PF_long_string);
    RUN_TEST(test_puts_PF_loremIpsum);
    UNITY_END();
}