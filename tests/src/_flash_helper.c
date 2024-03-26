#include <_flash_helper.h>
#include <stdlib.h>
//Unity
#include <unity.h>
//
#include <global_declarations.h>
// given data
#include "lorem_ipsum.h"

static uint32_t farProgMemStringUnderInspektion;
static uint16_t farMemStringIndex = 0;
static uint8_t buffer_index = 0;
static char * stdoutCopyBuffer;

void individualStartUpFlashHelperTests(void) { buffer_index = 0; }

void individualTearDownFlashHelperTests(void) {
    stdout->put = uartHelper->usartTransmitChar;
    free(stdoutCopyBuffer);
}

static int mockPutSmallBufferCompare(char c, FILE * stream) {
    if (verboseMode) { uartHelper->usartTransmitChar(c, stream); }
    stdoutCopyBuffer[buffer_index++] = c;
    return 0;
}

static int mockPutLoremIpsumCompare(char c, FILE * stream) {
    if (verboseMode) { uartHelper->usartTransmitChar(c, stream); }

    if (farMemStringIndex< LOREM_IPSUM_9_LENGHT) {
#ifdef __AVR_HAVE_ELPM__
        char expectedChar = pgm_read_byte_far(farProgMemStringUnderInspektion + farMemStringIndex);
#else
        char expectedChar = pgm_read_byte(farProgMemStringUnderInspektion + farMemStringIndex);
#endif
        TEST_ASSERT_EQUAL_CHAR(expectedChar, c);
    } else if (farMemStringIndex == LOREM_IPSUM_9_LENGHT)  {
        TEST_ASSERT_EQUAL_CHAR('\n', c);
    }

    farMemStringIndex++;
    return 0;
}

void test_createString_P(void) {
    // Given // When
    char * result = flashHelper->createString_P(addressOf(*PSTR("Test string")), flashHelper);
    // Then
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Test string", result);
    // Cleanup
    free(result);
}

void test_createString_P_null_helper(void) {
    // Given // When
    char * result = flashHelper->createString_P(addressOf(*PSTR("Test string")), NULL);
    // Then
    TEST_ASSERT_NULL(result);
}

void test_createString_P_empty_string(void) {
    // Given // When
    char * result = flashHelper->createString_P(addressOf(*PSTR("")), flashHelper);
    // Then
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("", result);
    // Cleanup
    free(result);
}
#if defined(__AVR_HAVE_ELPM__)
void test_createString_P_long_string(void) {
    // Given // When
    char * result = flashHelper->createString_P(addressOf(loremIpsumProgMem1kB), flashHelper);
    // Then
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING(loremIpsumRam1kB, result);
    // Cleanup
    free(result);
}
#else
void test_createString_P_long_string(void) {
    // Given // When
    char * result = flashHelper->createString_P(addressOf(loremIpsumProgMemUintMax), flashHelper);
    // Then
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING(loremIpsumRamUintMax, result);
    // Cleanup
    free(result);
}
#endif

void test_copyString_P(void) {
    // Given
    char dest[50];
    // When
    flashHelper->copyString_P(dest, addressOf(*PSTR("Test string")));
    // Then
    TEST_ASSERT_NOT_NULL(dest);
    TEST_ASSERT_EQUAL_STRING("Test string", dest);
}

// makes no sense at all because strcpy_PF for example will still fetch strings with Adressvalue 0, it should be the bootloader
//void test_copyString_P_null_source(void) {}

void test_copyString_P_null_dest(void) {
    // Given // When
    flashHelper->copyString_P(NULL, addressOf(*PSTR("Test string")));
    // Then
    // No assertion needed as this should cause a segmentation fault
}

void test_copyString_P_empty_string(void) {
    // Given
    char dest[50];
    // When
    flashHelper->copyString_P(dest, addressOf(*PSTR("")));
    // Then
    TEST_ASSERT_NOT_NULL(dest);
    TEST_ASSERT_EQUAL_STRING("", dest);
}

#define BUFFERSIZE_FLASHHELPER_TEST (uint8_t) UINT8_MAX
#if defined(__AVR_HAVE_ELPM__)
void test_copyString_P_long_string(void) {
    // Given
    char dest[1024];
    // When
    flashHelper->copyString_P(dest, addressOf(loremIpsumProgMem1kB));
    // Then
    TEST_ASSERT_NOT_NULL(dest);
    TEST_ASSERT_EQUAL_STRING(loremIpsumRam1kB, dest);
}
#else
void test_copyString_P_long_string(void) {
    // Given
    char * buffer = calloc(BUFFERSIZE_FLASHHELPER_TEST, sizeof(char));
    TEST_ASSERT_NOT_NULL( buffer);
    // When
    flashHelper->copyString_P(buffer , addressOf(loremIpsumProgMemUintMax));
    // Then
    TEST_ASSERT_NOT_NULL(buffer );
    TEST_ASSERT_EQUAL_STRING(loremIpsumRamUintMax, buffer );
    free(buffer);
//    HeapManagementHelper * heapManagementHelper = dOS_initHeapManagementHelper();
//    printf("free Memory is: %d\n",heapManagementHelper->getFreeMemory() );
//    free(heapManagementHelper);
}
#endif


void test_putString_P(void) {
    // Given
    stdoutCopyBuffer = calloc(BUFFERSIZE_FLASHHELPER_TEST, sizeof(char));
    TEST_ASSERT_NOT_NULL(stdoutCopyBuffer);
    stdout->put = mockPutSmallBufferCompare;
    // When
    flashHelper->putString_P(addressOf(*PSTR("Test string")));
    stdout->put = uartHelper->usartTransmitChar;
    stdoutCopyBuffer[buffer_index] = '\0';
    // Then
    char * testString = "Test string\n";
    for (uint8_t i = 0; i < BUFFERSIZE_FLASHHELPER_TEST; i++) {
        if (i < buffer_index) {
            TEST_ASSERT_EQUAL_CHAR(testString[i], stdoutCopyBuffer[i]);
        } else {
            TEST_ASSERT_EQUAL_CHAR('\0', stdoutCopyBuffer[i]);
        }
    }
}

//void test_putString_P_null_string(void) {}

void test_putString_P_empty_string(void) {
    // Given
    stdoutCopyBuffer = calloc(BUFFERSIZE_FLASHHELPER_TEST, sizeof(char));
    stdout->put = mockPutSmallBufferCompare;
    // When
    flashHelper->putString_P(addressOf(*PSTR("")));
    stdoutCopyBuffer[buffer_index] = '\0';
    // Then
    char * testString = "\n";
    for (uint8_t i = 0; i < BUFFERSIZE_FLASHHELPER_TEST; i++) {
        if (i < buffer_index) {
            TEST_ASSERT_EQUAL_CHAR(testString[i], stdoutCopyBuffer[i]);
        } else {
            TEST_ASSERT_EQUAL_CHAR('\0', stdoutCopyBuffer[i]);
        }
    }
}
//
void test_putString_P_long_string(void) {
    // Given
    stdout->put = mockPutLoremIpsumCompare;
    farMemStringIndex = 0;
    farProgMemStringUnderInspektion = addressOf(loremIpsum9);
    // When
    puts_P(loremIpsum9);

    flashHelper->putString_P(farProgMemStringUnderInspektion);
}

void test_lengthOfString_P(void) {
    // TODO: Write this test
}

void test_readByte_P(void) {
    // TODO: Write this test
}

void test_compareString_P(void) {
    // TODO: Write this test
}

void test_createFileString_P(void) {
    // TODO: Write this test
}

void test_putFileString_P(void) {
    // TODO: Write this test
}

void test_getOrPutDosMessage(void) {
    // TODO: Write this test
}

void runFlashHelperTests(void) {


    setUpIndividual = individualStartUpFlashHelperTests;
    tearDownIndividual = individualTearDownFlashHelperTests;

    UNITY_BEGIN();
    RUN_TEST(test_createString_P);
//    RUN_TEST(test_createString_P_null_flashString);
    RUN_TEST(test_createString_P_null_helper);
    RUN_TEST(test_createString_P_empty_string);
    RUN_TEST(test_createString_P_long_string);


    RUN_TEST(test_copyString_P);
//    RUN_TEST(test_copyString_P_null_source);
    RUN_TEST(test_copyString_P_null_dest);
    RUN_TEST(test_copyString_P_empty_string);
    RUN_TEST(test_copyString_P_long_string);


    RUN_TEST(test_putString_P);
//    RUN_TEST(test_putString_P_null_string);
    RUN_TEST(test_putString_P_empty_string);
    RUN_TEST(test_putString_P_long_string);


    RUN_TEST(test_lengthOfString_P);
    RUN_TEST(test_readByte_P);
    RUN_TEST(test_compareString_P);
    RUN_TEST(test_createFileString_P);
    RUN_TEST(test_putFileString_P);
    RUN_TEST(test_getOrPutDosMessage);
    UNITY_END();
    setUpIndividual = NULL;
    tearDownIndividual = NULL;
}

// running as isolated test for small devices
#if defined(DWARF_ISOLATED_TEST)

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

McuClock * mcuClock;
InputQueue * inputQueue;
HeapManagementHelper * heapHelper;

void setup(void);

void adjustTo1Sec(void);

void printToSerialOutput(void);

void freeAll(char * memoryString, char * formatString, char * timeStamp);

int main(void) {

    setup();
    sei();
    printToSerialOutput();
    while (1) {

        sleep_mode();
        adjustTo1Sec();

        if ((uint8_t) time(NULL) != lastTime) {
            lastTime = time(NULL);


            puts_P(PSTR(
                           "\n              Run Tests\n\n"
                           "Add character v before number for verbose mode.\n\n"
                           "1. run Test\n"
                           "2. print Memory again\n"));
            //do it with stdio function
            uint8_t menu = 0;
            char input[5];//v three digits and \n
            if (scanf("%s", input) == 1) {
                if (input[0] == 'v') {
                    verboseMode = 1;
                    printf("set verbose");
                    menu = strtol(input + 1, NULL, 10);
                } else {
                    verboseMode = 0;
                    menu = strtol(input, NULL, 10);
                }
            }
            switch (menu) {
                case 1:
                    runFlashHelperTests();
                    break;
                case 2:{
                    printToSerialOutput();
                    break;
                }
                default:
                    break;
            }
        }
    }
}

ISR(TIMER2_OVF_vect) { adjustCounter++; }


#ifdef __AVR_ATmega328P__

ISR(USART_RX_vect) { inputQueue->enqueue(UDR0, inputQueue); }
#elif  __AVR_ATmega2560__
ISR(USART0_RX_vect) { inputQueue->enqueue(UDR0, inputQueue); }
#endif


void printToSerialOutput(void) {


    int16_t memoryAmount = heapHelper->getFreeMemory();

    char * timestamp = ctime(NULL);
    char * memoryString = flashHelper->getOrPutDosMessage(FREE_MEMORY_STRING, 1, flashHelper);
    char * formatString = flashHelper->getOrPutDosMessage(TIMESTAMP_STRING_NUMBER_LF_FORMATSTRING, 1, flashHelper);
    if (!(memoryString && formatString && timestamp)) {
        freeAll(memoryString, formatString, timestamp);
        return;
    }
    printf(formatString, timestamp, memoryString, memoryAmount);
    freeAll(memoryString, formatString, timestamp);

}

void adjustTo1Sec(void) {
    if (adjustCounter == adjustToSecondValue) {
        mcuClock->incrementClockOneSec();
        adjustCounter = 0;
    }
}

void freeAll(char * memoryString, char * formatString,
             char * timeStamp) {
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
    heapHelper = dOS_initHeapManagementHelper();
    flashHelper = dOS_initFlashHelper(0);

    //setting up stdout and stdin
    fdevopen(uartHelper->usartTransmitChar, get_char);

    // Enable receiver and transmitter and Interrupt additionally
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}
#endif