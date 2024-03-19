#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#ifdef __AVR_HAVE_ELPM__
#include <avr/pgmspace.h>
#endif

//DwarfOS
#include <dwarf-os/setup.h>
#include <dwarf-os/ascii_helper.h>
#include <dwarf-os/mcu_clock.h>
#include <dwarf-os/uart_helper.h>
#include <dwarf-os/heap_management_helper.h>
#include <dwarf-os/time.h>
#include <dwarf-os/flash_helper.h>

//the clock has states, instance it one time and hold a reference somewhere if you use it
McuClock * mcuClock;
// we want to call the test function only one time per second
uint8_t lastTime;

// we can use the CPU Frequency as a clock with very less accuracy just for logging purpose
#ifndef DWARFOS_WATCH_QUARTZ
volatile uint8_t adjustCounter = 0;
const uint8_t adjustValue = ADJUST_TO_SECOND_VALUE;
#endif /* DWARFOS_WATCH_QUARTZ */

void adjustTo1Sec(void);

//
void testOSMethod(void);


int main(void) {

    setupMcu(&mcuClock);
    //global Interrupts on, could be moved to setup if you don't have other tasks before entering the loop
    sei();

    while (1) {

        sleep_mode();

#ifndef DWARFOS_WATCH_QUARTZ
        adjustTo1Sec();
#endif /* DWARFOS_WATCH_QUARTZ */

        // for the example its enough to show you something every second, else we go immediately back to sleep
        if ((uint8_t) time(NULL) != lastTime) {

            lastTime = time(NULL);
            testOSMethod();
        }
    }
}




// This overflow interrupt is connected to counter 2, but counter 2 runs on the system clock
// for real time functionality you need a watch quartz at the TOSC1 and TOSC2 pins and change the setup to use it
ISR(TIMER2_OVF_vect) {
#ifdef DWARFOS_WATCH_QUARTZ
    mcuClock->incrementClockOneSec();
}
#else
    adjustCounter++;
}

// counter overflow in this setup is connected to system clock
// 16Mhz / (pre scaling 1024 x overflow interrupt 256 x 61) = ca 1.0001hz, good enough for logging
void adjustTo1Sec(void) {
    if (adjustCounter == adjustValue) {
        mcuClock->incrementClockOneSec();
        adjustCounter = 0;
    }
}

#endif /* DWARFOS_WATCH_QUARTZ */



void freeAll(HeapManagementHelper * heapHelper, AsciiHelper * asciiHelper, FlashHelper * flashHelper,
             UartHelper * uartHelper, char * memoryAmountString, char * memoryString) {
    free(heapHelper);
    free(flashHelper);
    free(asciiHelper);
    free(uartHelper);
    free(memoryAmountString);
    free(memoryString);
}

// example placement of string in Progmem
#define MEMORY_STRING_LENGTH 26
const __attribute__((__progmem__)) char memoryStringOnFlash[MEMORY_STRING_LENGTH] = ": free Memory is (byte): ";
#define LONG_LOCATION_126_STRING_LENGTH 1300
const __attribute__((__progmem__)) char longLocation_126[LONG_LOCATION_126_STRING_LENGTH] = "You are on the edge of a breath-taking view.  Far below you is an \nactive volcano, from which great gouts of molten lava come surging \nout, cascading back down into the depths. The glowing rock fills the \nfarthest reaches of the cavern with a blood-red glare, giving \neverything an eerie, macabre appearance.\nThe air is filled with flickering sparks of ash and a heavy smell of \nbrimstone.  The walls are hot to the touch, and the thundering of the \nvolcano drowns out all other sounds.  Embedded in the jagged roof far \noverhead are myriad formations composed of pure white alabaster, which \nscatter their murky light into sinister apparitions upon the walls.\nTo one side is a deep gorge, filled with a bizarre chaos of tortured \nrock which seems to have been crafted by the Devil Himself.  An \nimmense river of fire crashes out from the depths of the volcano, \nburns its way through the gorge, and plummets into a bottomless pit \nfar off to your left.  \nTo the right, an immense geyser of blistering steam erupts \ncontinuously from a barren island in the center of a sulfurous lake, \nwhich bubbles ominously. The far right wall is aflame with an \nincandescence of its own, which lends an additional infernal splendor \nto the already hellish scene.  \nA dark, foreboding passage exits to the south.\n";
#define ACTION_142_STRING_LENGTH 1430
const __attribute__((__progmem__)) char action_142[ACTION_142_STRING_LENGTH] = "If you want to end your adventure early, say \"quit\".  To suspend your \nadventure such that you can continue later say \"suspend\" (or \"pause\" \nor \"save\").  To load a previously saved game, say 'load' or 'restore'.  \nTo see how well you're doing, say \"score\".  To get full credit for a \ntreasure, you must have left it safely in the building, though you get \npartial credit just for locating it. You lose points for getting \nkilled, or for quitting, though the former costs you more. \nThere are also points based on how much (If any) of the cave you've \nmanaged to explore;  in particular, there is a large bonus just for \ngetting in (to distinguish the beginners from the rest of the pack), \nand there are other ways to determine whether you've been through some \nof the more harrowing sections. \nIf you think you've found all the treasures, just keep exploring for a \nwhile.  If nothing interesting happens, you haven't found them all \nyet.  If something interesting DOES happen, it means you're getting a \nbonus and have an opportunity to garner many more points in the \nmaster's section.\nI may occasionally offer hints in you seem to be having trouble.  If I \ndo, I'll warn you in advance how much it will affect your score to \naccept the hints.  Finally, to save paper, you may specify \"brief\", \nwhich tells me never to repeat the full description of a place unless \nyou explicitly ask me to.\n";

void testOSMethod(void) {

    HeapManagementHelper * heapHelper = dOS_initHeapManagementHelper();
    if (!heapHelper) { return; }

    // result 2003 byte on AtMega328p, overhead incl heapHelper only 45byte
    int16_t memoryAmount = heapHelper->getFreeMemory();

    AsciiHelper * asciiHelper = dOS_initAsciiHelper();
    FlashHelper * flashHelper = dOS_initFlashHelper();
    UartHelper * uartHelper = dOS_initUartHelper();
    char * memoryAmountString = calloc(5, sizeof(char));
    char * memoryString = malloc(MEMORY_STRING_LENGTH + 1);

    if (!(asciiHelper && flashHelper && uartHelper && memoryString && memoryAmountString)) {
        freeAll(heapHelper, asciiHelper, flashHelper, uartHelper, memoryAmountString, memoryString);
        return;
    }
    asciiHelper->integerToAscii(memoryAmountString, memoryAmount, 4, 0);

    // cou can get rid of the ugly ifdef when you can decide on which device your code is running
#ifdef __AVR_HAVE_ELPM__
    flashHelper->loadString_PF(memoryString, pgm_get_far_address(memoryStringOnFlash));
#else
    flashHelper->loadNearStringFromFlash(memoryString, memoryStringOnFlash);
#endif
    // you can find easily memory leaks if you make such a check at several places in your code
    if (lastTime % 2) {

        char * actionString = malloc(LONG_LOCATION_126_STRING_LENGTH + 1);
#ifdef __AVR_HAVE_ELPM__
        flashHelper->loadString_PF(actionString,  pgm_get_far_address(longLocation_126));
#else
        flashHelper->loadNearStringFromFlash(actionString, longLocation_126);
#endif
        uartHelper->usartTransmitString(actionString);
        free(actionString);
        //need a small delay until we go to sleep, that the receiver can read the end of our message,
        // otherwise only \r is read not \n
        uartHelper->usartTransmitChar('\0');
    } else {
        char * action2String = malloc(ACTION_142_STRING_LENGTH + 1);
#ifdef __AVR_HAVE_ELPM__
        flashHelper->loadString_PF(action2String,  pgm_get_far_address(action_142));
#else
        flashHelper->loadString_P(action2String, action_142);
#endif

        uartHelper->usartTransmitString(action2String);
        free(action2String);
        //need a small delay until we go to sleep, that the receiver can read the end of our message,
        // otherwise only \r is read not \n
        uartHelper->usartTransmitChar('\0');
    }
    uartHelper->sendMsgWithTimestamp(2, (char * []) {memoryString, memoryAmountString});

    freeAll(heapHelper, asciiHelper, flashHelper, uartHelper, memoryAmountString, memoryString);
}