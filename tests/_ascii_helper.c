#include "_ascii_helper.h"

void setUp(void) {}
void tearDown(void) {

}
int main(void) {
    UNITY_BEGIN();


    RUN_TEST(test_integerToAscii);

    return UNITY_END();
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