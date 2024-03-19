#include <unity.h>
#include "dwarf-os/ascii_helper.h"
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {
}

void test_integerToAscii(void) {
    char resultArray[10]; // Adjust the size as per your requirement
    uint32_t num = 12345;
    uint8_t size = 5;
    uint8_t position = 0;

    AsciiHelper * asciiHelper = dOS_initAsciiHelper();

    // Call the function
    asciiHelper->integerToAscii(resultArray, num, size, position);

    // Check the result
    TEST_ASSERT_EQUAL_STRING("12345", resultArray);
}

void test_concatStrings_TwoStrings(void) {
    char * strings[] = {"Hello, ", "world!"};
    AsciiHelper * asciiHelper = dOS_initAsciiHelper();


    char * result = asciiHelper->concatStrings(2, strings);

    // Verify the result is not NULL
    TEST_ASSERT_NOT_NULL(result);

    // Verify the concatenated string is "Hello, world!"
    TEST_ASSERT_EQUAL_STRING("Hello, world!", result);

    // Clean up memory
    free(result);
}
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_integerToAscii);
    RUN_TEST(test_concatStrings_TwoStrings);


    return UNITY_END();
}