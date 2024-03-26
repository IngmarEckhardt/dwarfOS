#ifndef DWARF_MCU_TEST_GLOBAL_DECLARATIONS_H
#define DWARF_MCU_TEST_GLOBAL_DECLARATIONS_H
#include <dwarf-os/uart_helper.h>

void (* setUpIndividual)(void);
void (* tearDownIndividual)(void);

void setUp(void);
void tearDown(void);

UartHelper * uartHelper;
#endif //DWARF_MCU_TEST_MCU_TEST_RUNNER_ELPM_H
