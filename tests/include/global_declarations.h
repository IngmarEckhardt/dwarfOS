#ifndef DWARF_MCU_TEST_GLOBAL_DECLARATIONS_H
#define DWARF_MCU_TEST_GLOBAL_DECLARATIONS_H
#include <dwarf-os/uart_helper.h>
#include <dwarf-os/flash_helper.h>
#include <dwarf-os/heap_management_helper.h>
void (* setUpIndividual)(void);
void (* tearDownIndividual)(void);

void setUp(void);
void tearDown(void);
uint8_t verboseMode;
UartHelper * uartHelper;
FlashHelper * flashHelper;
HeapManagementHelper * heapHelper;
#endif //DWARF_MCU_TEST_GLOBAL_DECLARATIONS_H
