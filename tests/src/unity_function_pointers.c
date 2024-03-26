#include <global_declarations.h>
#include <stddef.h>

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