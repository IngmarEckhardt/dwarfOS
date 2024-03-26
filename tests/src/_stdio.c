#include <_stdio.h>
// standard libs
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
// KISS Workaround to work with 2 given setUp, tearDown functions from unity, but the use case to have different files with test suites
#include <global_declarations.h>

// given data
#include "lorem_ipsum.h"
#if !defined(DWARF_ISOLATED_TEST)

#else
UartHelper * uartHelper;
#endif
static uint8_t verbose = 0;

#define BUFFERSIZE (uint8_t) UINT8_MAX
static uint32_t farProgMemStringUnderInspektion;
static uint16_t farMemStringIndex;
static uint8_t buffer_index = 0;
static char * stdoutCopyBuffer;

void tearDownStdIo(void) { stdout->put = uartHelper->usartTransmitChar; }


static int mockPutSmallBufferCompare(char c, FILE * stream) {
    if (verbose) { uartHelper->usartTransmitChar(c, stream); }
    stdoutCopyBuffer[buffer_index++] = c;
    return 0;
}

static int mockPutLoremIpsumCompare(char c, FILE * stream) {
    if (verbose) { uartHelper->usartTransmitChar(c, stream); }
#ifdef __AVR_HAVE_ELPM__
    char expectedChar = pgm_read_byte_far(farProgMemStringUnderInspektion + farMemStringIndex);
#else
    char expectedChar = pgm_read_byte(farProgMemStringUnderInspektion + farMemStringIndex);
#endif
    TEST_ASSERT_EQUAL_CHAR(expectedChar, c);
    farMemStringIndex++;
    return 0;
}

void test_puts_PF(void) {
    // given
    stdoutCopyBuffer = calloc(BUFFERSIZE, sizeof(char));
    buffer_index = 0;
    stdout->put = mockPutSmallBufferCompare;
    uint32_t farPointerToString = addressOf(*PSTR("Test string"));
    // when
    puts_PF(farPointerToString);
    stdoutCopyBuffer[buffer_index] = '\0';
    // then
    char* testString = "Test string";
    for (uint8_t i = 0; i < BUFFERSIZE; i++) {
        if (i < buffer_index) {
            TEST_ASSERT_EQUAL_CHAR(testString[i], stdoutCopyBuffer[i]);
        } else {
            TEST_ASSERT_EQUAL_CHAR('\0', stdoutCopyBuffer[i]);
        }
    }
}

void test_puts_PF_empty_string(void) {
    // given
    stdoutCopyBuffer = calloc(BUFFERSIZE, sizeof(char));
    buffer_index = 0;
    stdout->put = mockPutSmallBufferCompare;
    uint32_t farPointerToString = addressOf(*PSTR(""));
    // when
    puts_PF(farPointerToString);
    stdoutCopyBuffer[buffer_index] = '\0';
    // then
    char* testString = "";
    for (uint8_t i = 0; i < BUFFERSIZE; i++) {
        if (i < buffer_index) {
            TEST_ASSERT_EQUAL_CHAR(testString[i], stdoutCopyBuffer[i]);
        } else {
            TEST_ASSERT_EQUAL_CHAR('\0', stdoutCopyBuffer[i]);
        }
    }
}

void test_puts_PF_long_string(void) {
    // given
    stdoutCopyBuffer = calloc(BUFFERSIZE, sizeof(char));
    buffer_index = 0;
    stdout->put = mockPutSmallBufferCompare;
    uint32_t farPointerToString = addressOf(*PSTR("This is a long test string"));
    // when
    puts_PF(farPointerToString);
    // then
    char* testString = "This is a long test string";
    for (uint8_t i = 0; i < BUFFERSIZE; i++) {
        if (i < buffer_index) {
            TEST_ASSERT_EQUAL_CHAR(testString[i], stdoutCopyBuffer[i]);
        } else {
            TEST_ASSERT_EQUAL_CHAR('\0', stdoutCopyBuffer[i]);
        }
    }
}

void test_puts_PF_loremIpsum(void) {
    //given
    farProgMemStringUnderInspektion = addressOf(loremIpsum9);
    stdout->put = mockPutLoremIpsumCompare; // here are the asserts
    farMemStringIndex = 0;
    // when
    puts_PF(farProgMemStringUnderInspektion);
}

void runPutsPFTests(uint8_t verboseMode) {
    verbose = verboseMode;
    tearDownIndividual = tearDownStdIo;
    UNITY_BEGIN();
    RUN_TEST(test_puts_PF);
    RUN_TEST(test_puts_PF_empty_string);
    RUN_TEST(test_puts_PF_long_string);
    RUN_TEST(test_puts_PF_loremIpsum);
    UNITY_END();
    tearDownIndividual = NULL;
}


// running as isolated test for small devices
#if defined(DWARF_ISOLATED_TEST) && !defined(__AVR_HAVE_ELPM__)

#include <dwarf-os/mcu_clock.h>
#include <dwarf-os/input_queue.h>
#include <dwarf-os/heap_management_helper.h>
#include <dwarf-os/setup.h>
#include <dwarf-os/time.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

const uint8_t adjustToSecondValue = ADJUST_TO_SECOND_VALUE;
uint8_t lastTime;
volatile uint8_t adjustCounter;

uint8_t verboseMode;

McuClock * mcuClock;
UartHelper * uartHelper;
InputQueue * inputQueue;
void setup(void);

void adjustTo1Sec(void);

void printToSerialOutput(void);

void freeAll(HeapManagementHelper * helper, FlashHelper * pHelper, char * memoryString, char * formatString,
             char * timeStamp);

int main(void) {

    setup();
    sei();

    while (1) {

        sleep_mode();
        adjustTo1Sec();
        if ((uint8_t) time(NULL) != lastTime) {
            lastTime = time(NULL);
            printToSerialOutput();

            puts_P(PSTR(
                    "\n              Run Tests\n\n"
                    "Add character v before number for verbose mode.\n\n")));
            //do it with stdio function
            uint8_t menu = 0;
            char input[5];//v three digits and \n
            if (scanf("%s", input) == 1) {
                if (input[0] == 'v') {
                    verboseMode = 1;
                    menu = strtol(input + 1, NULL, 10);
                } else {
                    verboseMode = 0;
                    menu = strtol(input, NULL, 10);
                }
            }
            switch (menu) {
                case 1:
                    runPutsPFTests(verboseMode);
                    break;
                default:
                    break;
            }
        }
    }
}

ISR(TIMER2_OVF_vect) { adjustCounter++; }


#ifdef __AVR_ATmega328P__
ISR(USART_RX_vect) { inputQueue->enqueue(UDR0, inputQueue); }
#endif


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
void setup(void) {
    setupMcu(&mcuClock); // general setup DwarfOS

    uartHelper = dOS_initUartHelper();
    inputQueue = dOS_initInputQueue();

    //setting up stdout and stdin
    fdevopen(uartHelper->usartTransmitChar, get_char);

    // Enable receiver and transmitter and Interrupt additionally
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}
#elif defined(DWARF_ISOLATED_TEST) && defined(__AVR_HAVE_ELPM__)
int main(void){}; // is there a need to implement tests for isolated ELPM tests, it is done in the test runner?
#endif