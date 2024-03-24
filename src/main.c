#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

//DwarfOS
#include <dwarf-os/setup.h>
#include <dwarf-os/ascii_helper.h>
#include <dwarf-os/mcu_clock.h>
#include <dwarf-os/uart_helper.h>
#include <dwarf-os/heap_management_helper.h>
#include <dwarf-os/time.h>
#include <dwarf-os/flash_helper.h>
#include <dwarf-os/readme_data.h>
#include <dwarf-os/input_queue.h>

// The clock has states, instantiate it once and maintain a reference if you use it
McuClock * mcuClock;
// The clock has states, instantiate it once and maintain a reference if you use it
uint8_t lastTime;

// We can use the CPU Frequency as a clock with low accuracy for logging purposes
#ifndef DWARFOS_WATCH_QUARTZ
volatile uint8_t adjustCounter = 0;
const uint8_t adjustValue = ADJUST_TO_SECOND_VALUE;

void adjustTo1Sec(void);

#endif /* DWARFOS_WATCH_QUARTZ */


void sendMemoryAmountSmallModules(void);

void setupStdInOut(void);

void testOSMethod(void);


int main(void) {
    setupMcu(&mcuClock); // Perform general OS setup
    sendMemoryAmountSmallModules(); // Display the minimal amount of RAM usage
    setupStdInOut();
    sei(); // Enable global interrupts, this could be moved to setup if there are no other tasks before entering the loop
    while (1) {

        sleep_mode();

#ifndef DWARFOS_WATCH_QUARTZ
        adjustTo1Sec();
#endif /* DWARFOS_WATCH_QUARTZ */

        // For this example, we only need to execute an action once every second. Otherwise, we immediately return to sleep mode.
        if ((uint8_t) time(NULL) != lastTime) {
            lastTime = time(NULL);
            testOSMethod();
        }
    }
}
// This overflow interrupt is linked to counter 2, which operates on the system clock.
// For real-time functionality, a watch quartz is required at the TOSC1 and TOSC2 pins, and the setup needs to be adjusted accordingly.
ISR(TIMER2_OVF_vect) {
#ifdef DWARFOS_WATCH_QUARTZ
    mcuClock->incrementClockOneSec();
#else
    adjustCounter++;
#endif /* DWARFOS_WATCH_QUARTZ */
}

FlashHelper * flashHelper;

void printUserSelectedStringFromFile(void);

void testOSMethod(void) {
    /** Without a specified state, the helper autonomously determines whether to use near program memory getters for memory less than 64KB
    * or far getters (ELPM). */
    flashHelper = dOS_initFlashHelper(0);
    /** The addressOf macro autonomously determines how to retrieve the address. If state 1 is used on devices with ELPM,
    * this macro will still fetch far addresses. Although this will work, it would be slower than using the near address.
    * Therefore, it is recommended to use near addresses without the macro at this point.
    *
    * As it uses stdout, sending the string with putString_p does not consume any memory. DwarfOS has its own
    * puts_PF implementation to facilitate this for devices that support ELPM. */
    flashHelper->putString_P(addressOf(readme_data));
    sendMemoryAmountSmallModules();
    /**
     * The following function will prompt the user to select a string from the file short_locations and actions.
     * The user can select a string by entering a number. The function will then print the selected string.
     * */
    printUserSelectedStringFromFile();
}

/**
 * Each array in these files consumes 2 bytes of RAM. You have the flexibility to decide the number of arrays
 * you want to create when using the pgm_textfiles_generator. Each getter also consumes 2 bytes of RAM. However,
 * a text file with 3 arrays of strings only uses 10 bytes of RAM. The function PutFileString_P does not consume
 * any memory because it utilizes stdout for output.
 * */

// Function declarations are provided here for the functions that will be used in the testOSMethod. Instead of including them in a header file,
// the corresponding source files are directly included when DwarfOS is not built as a library along with this main file.
int16_t putFileStrAction(FlashHelper * helper, uint8_t actionNumber);

int16_t putFileStrShortLocation(FlashHelper * helper, uint8_t shortLocationNumber);

void printUserSelectedStringFromFile(void) {
    flashHelper->putString_P(
            addressOf(*(PSTR("Enter the number of the desired string from file 'short_locations':\n"))));
    int file;
    if (scanf("%d", &file) != 1) { file = 0; }
    if (file) {
        // A file with an array containing index information will search through this information
        // and return the string that matches the given number.
        putFileStrShortLocation(flashHelper, 121);
    }
    sendMemoryAmountSmallModules();
#ifdef __AVR_HAVE_ELPM__ // With 'actions', program memory would be too full for devices without ELPM support
    flashHelper->putString_P(addressOf(*(PSTR("Enter the number of the desired string from file 'actions':\n"))));
    if (scanf("%d", &file) != 1) { file = 0; }
    if (file) {
        // A file without an array index will return strings from the first array position in this case.
        // For a more intelligent selection, you could enhance the implementation.
        putFileStrAction(flashHelper, file - 1);
    }
#endif
}

/**
 * The following 30 lines demonstrate how to use helper functions to send information via UART serial output.
 * This is achieved without the need for large libraries, instead utilizing very small modules.
 *
 * */

void freeAll(HeapManagementHelper * heapHelper, AsciiHelper * asciiHelper, FlashHelper * flHelper,
             UartHelper * uartHelper, char * memoryAmountString) {
    free(heapHelper);
    free(flHelper);
    free(asciiHelper);
    free(uartHelper);
    free(memoryAmountString);
}

void sendMemoryAmountSmallModules(void) {
    HeapManagementHelper * heapHelper = dOS_initHeapManagementHelper();
    if (!heapHelper) { return; }
    int16_t memoryAmount = heapHelper->getFreeMemory();

    AsciiHelper * asciiHelper = dOS_initAsciiHelper();
    FlashHelper * flHelper = dOS_initFlashHelper(2);
    UartHelper * uartHelper = dOS_initUartHelper();
    char * memoryAmountString = calloc(5, sizeof(char));

    if (!(asciiHelper && flHelper && uartHelper && memoryAmountString)) {
        freeAll(heapHelper, asciiHelper, flHelper, uartHelper, memoryAmountString);
        return;
    }
    char * memoryString = flHelper->getOrPutDosMessage(FREE_MEMORY_STRING, 1, flHelper);
    asciiHelper->integerToAscii(memoryAmountString, memoryAmount, 4, 0);
    uartHelper->sendMsgWithTimestamp(2, (char * []) {memoryString, memoryAmountString});
    freeAll(heapHelper, asciiHelper, flHelper, uartHelper, memoryAmountString);
    free(memoryString);
}

/**
 * The following 20 lines are dedicated to setting up stdin with an input queue that starts with a size of 7 bytes,
 * and stdout with a UART helper.
 *
 * */

InputQueue * inputQueue;
UartHelper * uartHelper;

#ifdef __AVR_ATmega328P__ // For other devices, you need to manually find the appropriate interrupt vector.
ISR(USART_RX_vect) { inputQueue->enqueue(UDR0, inputQueue); }
#elif __AVR_ATmega2560__

ISR(USART0_RX_vect) { inputQueue->enqueue(UDR0, inputQueue); }

#endif

int get_char(FILE * stream) { return inputQueue->get_char(inputQueue, 1); }

void setupStdInOut(void) {
    uartHelper = dOS_initUartHelper();
    inputQueue = dOS_initInputQueue();
    //setting up stdout and stdin
    fdevopen(uartHelper->usartTransmitChar, get_char);
    // Enable receiver and transmitter and Interrupt additionally
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

/**
 * This function is utilized to obtain a seconds value with relatively low accuracy. It is primarily used for logging
 * and testing purposes in the absence of a 32kHz watch quartz.
 *
 * */

#ifndef DWARFOS_WATCH_QUARTZ

// In this setup, the counter overflow is linked to the system clock.
// With a clock speed of 16MHz, and considering the pre-scaling factor of 1024, an overflow interrupt of 256, and a multiplier of 61,
// the resulting frequency is approximately 1.0001Hz. This level of accuracy is sufficient for logging purposes.
void adjustTo1Sec(void) {
    if (adjustCounter == adjustValue) {
        mcuClock->incrementClockOneSec();
        adjustCounter = 0;
    }
}

#endif