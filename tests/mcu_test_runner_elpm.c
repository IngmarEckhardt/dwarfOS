#if defined(__AVR_HAVE_ELPM__) && !defined(DWARF_ISOLATED_TEST) //test runner using way more than 64kB Program Memory

#include <string.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>

#include <dwarf-os/time.h>
#include <dwarf-os/mcu_clock.h>

#include <dwarf-os/input_queue.h>
#include <dwarf-os/setup.h>
#include <dwarf-os/heap_management_helper.h>


#include <_stdio.h>
#include <_flash_helper.h>
#include <dwarf-os/stdio.h>
#include <global_declarations.h>


McuClock * mcuClock;
InputQueue * inputQueue;
HeapManagementHelper * heapHelper;

const uint8_t adjustToSecondValue = ADJUST_TO_SECOND_VALUE;
uint8_t buffer_index = 0;
uint8_t lastTime = 0;
volatile uint8_t adjustCounter;

uint8_t verboseMode = 0;

void setup(void);

void adjustTo1Sec(void);

void printFreeMemoryAmountToSerialOutput(void);

void freeAll(char * memStrg, char * formatStrg, char * timeStamp);


int main(void) {

    setup();
    sei();

    while (1) {

        sleep_mode();
        adjustTo1Sec();
        if ((uint8_t) time(NULL) != lastTime) {
            lastTime = time(NULL);

            printFreeMemoryAmountToSerialOutput();
            puts_PF(addressOf(*PSTR(
                    "\n              Run Tests\n\n"
                    "Add character v before number for verbose mode.\n\n"
                    "1.     stdio Tests\n"
                    "2.     flash_helper Tests\n"
                    "0.     All\n\n")));

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
                case 1: {
                    runPutsPFTests();
                    break;
                }
                case 2: {
                    runFlashHelperTests();
                    break;
                }
                default: {
                    runPutsPFTests();
                    runFlashHelperTests();
                    break;
                }

            }
        }
    }
}

ISR(TIMER2_OVF_vect) { adjustCounter++; }

ISR(USART0_RX_vect) { inputQueue->enqueue(UDR0, inputQueue); }

// stdin with sleep mode during waiting for input
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

void freeAll(char * memStrg, char * formatStrg,
             char * timeStamp) {
    free(memStrg);
    free(formatStrg);
    free(timeStamp);
}


void printFreeMemoryAmountToSerialOutput(void) {
    char * timestamp = NULL;
    char * memoryString = NULL;
    char * formatString = NULL;

    int16_t memoryAmount = heapHelper->getFreeMemory();
    printf("free memory is %d", memoryAmount);
    timestamp = ctime(NULL);
    printf("timestamp is %s", timestamp);
    memoryString = flashHelper->getOrPutDosMessage(FREE_MEMORY_STRING, 1, flashHelper);
    printf("memory string is %s", memoryString);
    formatString = flashHelper->getOrPutDosMessage(TIMESTAMP_STRING_NUMBER_LF_FORMATSTRING, 1, flashHelper);
    printf("format string is %s", formatString);
    if (!(memoryString && formatString && timestamp)) {
        freeAll(memoryString, formatString, timestamp);
        return;
    } else {
        char * errorMsg = flashHelper->getOrPutDosMessage(FATAL_ERROR_STRING, 1, flashHelper);
        printf("%s%d",  errorMsg, 1);
        free(errorMsg);
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

#else
int main(void){};
#endif