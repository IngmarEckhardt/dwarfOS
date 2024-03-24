#ifdef __AVR_HAVE_ELPM__ //example uses way mor than 64kB Program Memory

#include <string.h>
#include <unity.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>

//DwarfOS
#include <dwarf-os/setup.h>
#include <dwarf-os/mcu_clock.h>
#include <dwarf-os/uart_helper.h>
#include <dwarf-os/flash_helper.h>
#include <dwarf-os/heap_management_helper.h>
#include <dwarf-os/stdio.h>
#include <dwarf-os/time.h>
#include <dwarf-os/input_queue.h>

#include "lorem_ipsum.h"

McuClock * mcuClock;
UartHelper * uartHelper;
InputQueue * inputQueue;

char * stdoutCopyBuffer;
#define BUFFERSIZE (uint8_t) UINT8_MAX
uint8_t buffer_index = 0;

const uint8_t adjustToSecondValue = ADJUST_TO_SECOND_VALUE;
uint8_t lastTime;
volatile uint8_t adjustCounter;

uint32_t farProgMemStringUnderInspektion;
uint16_t farMemStringIndex;

void setUp(void) {}

void tearDown(void) {stdout->put = uartHelper->usartTransmitChar;}

void setup(void);

void adjustTo1Sec(void);

void printToSerialOutput(void);

void freeAll(HeapManagementHelper * helper, FlashHelper * pHelper, char * memoryString, char * formatString,
             char * timeStamp);

void test_puts_PF(void);

void test_puts_PF_empty_string(void);

void test_puts_PF_long_string(void);
void test_puts_PF_loremIpsum(void);

int main(void) {

    setup();
    sei();

    while (1) {

        sleep_mode();
        adjustTo1Sec();
        if ((uint8_t) time(NULL) != lastTime) {
            lastTime = time(NULL);
            printToSerialOutput();
            printf("Enter 1 to run tests\n");
            //do it with stdio function
            int menu;
            if (scanf("%d", &menu) != 1) { menu = 0; }
            switch (menu) {
                case 1:
                    UNITY_BEGIN();
                    RUN_TEST(test_puts_PF);
                    RUN_TEST(test_puts_PF_empty_string);
                    RUN_TEST(test_puts_PF_long_string);
                    RUN_TEST(test_puts_PF_loremIpsum);
                    UNITY_END();
                    break;
                default:
                    break;
            }
        }
    }
}

ISR(TIMER2_OVF_vect) { adjustCounter++; }


// every time a new char is received, the interrupt is triggered and the char is put into the queue
ISR(USART0_RX_vect) { inputQueue->enqueue(UDR0, inputQueue); }


void printToSerialOutput(void) {
    HeapManagementHelper * heapHelper = dOS_initHeapManagementHelper();
    if (heapHelper) {
        int16_t memoryAmount = heapHelper->getFreeMemory();
        FlashHelper * flashHelper = dOS_initFlashHelper(0);
        char * timestamp = ctime(NULL);
        char * memoryString = flashHelper->getOrPutDosMessage(FREE_MEMORY_STRING, 1, flashHelper);
        char * formatString = flashHelper->getOrPutDosMessage(TIMESTAMP_STRING_NUMBER_LF_FORMATSTRING, 1, flashHelper);
        if (!(memoryString && formatString && flashHelper && timestamp)) {
            freeAll(heapHelper, flashHelper, memoryString, formatString, timestamp);
            return;
        }
        printf(formatString, timestamp, memoryString, memoryAmount);
        freeAll(heapHelper, flashHelper, memoryString, formatString, timestamp);
    }
}

void adjustTo1Sec(void) {
    if (adjustCounter == adjustToSecondValue) {
        mcuClock->incrementClockOneSec();
        adjustCounter = 0;
    }
}

void freeAll(HeapManagementHelper * helper, FlashHelper * pHelper, char * memoryString, char * formatString,
             char * timeStamp) {
    free(helper);
    free(pHelper);
    free(memoryString);
    free(formatString);
    free(timeStamp);
}

// StdIn - Buffer outflow
int get_char(FILE * stream) { return inputQueue->get_char(inputQueue, 1); }

int mockPutSmallBufferCompare(char c, FILE * stream) {
    uartHelper->usartTransmitChar(c, stream);

    stdoutCopyBuffer[buffer_index++] = c;
    return 0;
}
int mockPutLoremIpsumCompare(char c, FILE * stream) {
    uartHelper->usartTransmitChar(c, stream);

    // Read the next character from the far memory string
    char expectedChar = pgm_read_byte_far(farProgMemStringUnderInspektion + farMemStringIndex);

    // Assert that the transmitted character is the same as the expected character
    TEST_ASSERT_EQUAL_CHAR(expectedChar, c);

    // Move to the next character in the far memory string
    farMemStringIndex++;

    return 0;
}
void setup(void) {
    setupMcu(&mcuClock); // general setup DwarfOS

    uartHelper = dOS_initUartHelper();
    inputQueue = dOS_initInputQueue();

    //setting up stdout and stdin
    fdevopen(uartHelper->usartTransmitChar, get_char);

    // Enable receiver and transmitter and Interrupt additionally
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

void test_puts_PF(void) {
    stdoutCopyBuffer = calloc(BUFFERSIZE, sizeof(char));
    buffer_index = 0;
    stdout->put = mockPutSmallBufferCompare;

    // Create a far pointer to a string in program memory
    uint32_t farPointerToString = (uint32_t) PSTR("Test string");

    // Call the function to test
    puts_PF(farPointerToString);
    // Add null terminator to the stdoutCopyBuffer
    stdoutCopyBuffer[buffer_index] = '\0';


    // Check the output
    TEST_ASSERT_EQUAL_STRING("Test string", stdoutCopyBuffer);
    for (uint8_t i = buffer_index; i < BUFFERSIZE; i++) {
        TEST_ASSERT_EQUAL_CHAR('\0', stdoutCopyBuffer[i]);
    }
}

void test_puts_PF_empty_string(void) {
    stdoutCopyBuffer = calloc(BUFFERSIZE, sizeof(char));
    buffer_index = 0;
    stdout->put = mockPutSmallBufferCompare;

    // Create a far pointer to an empty string in program memory
    uint32_t farPointerToString = (uint32_t) PSTR("");

    // Call the function to test
    puts_PF(farPointerToString);
    // Add null terminator to the stdoutCopyBuffer
    stdoutCopyBuffer[buffer_index] = '\0';

    // Check the output
    TEST_ASSERT_EQUAL_STRING("", stdoutCopyBuffer);
    for (uint8_t i = buffer_index; i < BUFFERSIZE; i++) {
        TEST_ASSERT_EQUAL_CHAR('\0', stdoutCopyBuffer[i]);
    }
}

void test_puts_PF_long_string(void) {
    stdoutCopyBuffer = calloc(BUFFERSIZE, sizeof(char));
    buffer_index = 0;
    stdout->put = mockPutSmallBufferCompare;

    // Create a far pointer to a long string in program memory
    uint32_t farPointerToString = (uint32_t) PSTR("This is a long test string");
    // Call the function to test
    puts_PF(farPointerToString);

    // Check the output
    TEST_ASSERT_EQUAL_STRING("This is a long test string", stdoutCopyBuffer);
    for (uint8_t i = buffer_index; i < BUFFERSIZE; i++) {
        TEST_ASSERT_EQUAL_CHAR('\0', stdoutCopyBuffer[i]);
    }
}

void test_puts_PF_loremIpsum(void) {
    farProgMemStringUnderInspektion = addressOf(loremIpsum1);
    stdout->put = mockPutLoremIpsumCompare;
    farMemStringIndex = 0;
    // Call the function to test
    puts_PF(addressOf(loremIpsum1));
}

#else
int main(void){};
#endif