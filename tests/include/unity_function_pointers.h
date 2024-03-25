#ifndef DWARF_MCU_TEST_UNITY_FUNCTION_POINTERS_H
#define DWARF_MCU_TEST_UNITY_FUNCTION_POINTERS_H


void (* setUpIndividual)(void);
void (* tearDownIndividual)(void);

void setUp(void) {
    if (setUpIndividual != NULL) {
        setUpIndividual();
    }
}
void tearDown(void) {
    if (tearDownIndividual != NULL) {
        tearDownIndividual();
    }
}
#endif //DWARF_MCU_TEST_MCU_TEST_RUNNER_ELPM_H
