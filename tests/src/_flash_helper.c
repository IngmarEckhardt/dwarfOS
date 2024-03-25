#include <_flash_helper.h>
#include <stdlib.h>
//Unity
#include <unity.h>
// DwarfOS
#include <dwarf-os/flash_helper.h>
// given data
#include "lorem_ipsum.h"

uint8_t verbose = 0;

void setUp(void) {}

void tearDown(void) {}

void test_createString_P(void) {
    // Given
    FlashHelper * helper = dOS_initFlashHelper(0);
    // When
    char * result = helper->createString_P(addressOf(*PSTR("Test string")), helper);

    // Then
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Test string", result);

    // Cleanup
    free(result);
    free(helper);
}

void test_createString_P_null_flashString(void) {
    // Given
    FlashHelper * helper = dOS_initFlashHelper(0);
    // When
    char * result = helper->createString_P(0, helper);
    // Then
    TEST_ASSERT_NULL(result);
    // Cleanup
    free(helper);
}

void test_createString_P_null_helper(void) {
    // Given
    FlashHelper * helper = dOS_initFlashHelper(0);
    // When
    char * result = helper->createString_P(addressOf(*PSTR("Test string")), NULL);
    // Then
    TEST_ASSERT_NULL(result);
}

void test_createString_P_empty_string(void) {
    // Given
    FlashHelper * helper = dOS_initFlashHelper(0);
    // When
    char * result = helper->createString_P(addressOf(*PSTR("")), helper);
    // Then
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("", result);
    // Cleanup
    free(result);
    free(helper);
}

void test_createString_P_long_string(void) {
    // Given
    FlashHelper * helper = dOS_initFlashHelper(0);
    // When
    char * result = helper->createString_P(addressOf(loremIpsumProgMem1kB), helper);
    // Then
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING(loremIpsumRam1kB, result);
    // Cleanup
    free(result);
    free(helper);
}

void test_copyString_P(void) {
    // TODO: Write this test
}

void test_putString_P(void) {
    // TODO: Write this test
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

void runFlashHelperTests(uint8_t verboseMode) {
    verbose = verboseMode;
    UNITY_BEGIN();
    RUN_TEST(test_createString_P);
    RUN_TEST(test_createString_P_null_flashString);
    RUN_TEST(test_createString_P_null_helper);
    RUN_TEST(test_createString_P_empty_string);
    RUN_TEST(test_createString_P_long_string);


    RUN_TEST(test_copyString_P);
    RUN_TEST(test_putString_P);
    RUN_TEST(test_lengthOfString_P);
    RUN_TEST(test_readByte_P);
    RUN_TEST(test_compareString_P);
    RUN_TEST(test_createFileString_P);
    RUN_TEST(test_putFileString_P);
    RUN_TEST(test_getOrPutDosMessage);
    UNITY_END();
}