#include <_flash_helper.h>
#include <stdlib.h>
//Unity
#include <unity.h>
// KISS Workaround to work with 2 given setUp, tearDown functions from unity, but the use case to have different files with test suites
#include <global_declarations.h>
// DwarfOS
#include <dwarf-os/flash_helper.h>


// given data
#include "lorem_ipsum.h"

static FlashHelper * flashHelper;
static uint8_t verbose = 0;

static uint32_t farProgMemStringUnderInspektion;
static uint16_t farMemStringIndex;
static uint8_t buffer_index = 0;
static char * stdoutCopyBuffer;

void individualStartUpFlashHelperTests(void) { buffer_index = 0; }

void individualTearDownFlashHelperTests(void) {
    stdout->put = uartHelper->usartTransmitChar;
    free(stdoutCopyBuffer);
}

static int mockPutSmallBufferCompare(char c, FILE * stream) {
    if (verbose) { uartHelper->usartTransmitChar(c, stream); }
    stdoutCopyBuffer[buffer_index++] = c;
    return 0;
}

static int mockPutLoremIpsumCompare(char c, FILE * stream) {
    if (verbose) { uartHelper->usartTransmitChar(c, stream); }
#ifdef __AVR_HAVE_ELPM__
    char expectedChar = pgm_read_byte_far(farProgMemStringUnderInspektion + farMemStringIndex);
#else
    char expectedChar = pgm_read_byte(farProgMemStringUnderInspektion + farMemStringIndex);
#endif
    TEST_ASSERT_EQUAL_CHAR(expectedChar, c);
    farMemStringIndex++;
    return 0;
}

void test_createString_P(void) {
    // Given // When
    char * result = flashHelper->createString_P(addressOf(*PSTR("Test string")), flashHelper);
    // Then
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Test string", result);
    // Cleanup
    free(result);
}

void test_createString_P_null_helper(void) {
    // Given // When
    char * result = flashHelper->createString_P(addressOf(*PSTR("Test string")), NULL);
    // Then
    TEST_ASSERT_NULL(result);
}

void test_createString_P_empty_string(void) {
    // Given // When
    char * result = flashHelper->createString_P(addressOf(*PSTR("")), flashHelper);
    // Then
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("", result);
    // Cleanup
    free(result);
}

void test_createString_P_long_string(void) {
    // Given // When
    char * result = flashHelper->createString_P(addressOf(loremIpsumProgMem1kB), flashHelper);
    // Then
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING(loremIpsumRam1kB, result);
    // Cleanup
    free(result);
}

void test_copyString_P(void) {
    // Given
    char dest[50];
    // When
    flashHelper->copyString_P(dest, addressOf(*PSTR("Test string")));
    // Then
    TEST_ASSERT_NOT_NULL(dest);
    TEST_ASSERT_EQUAL_STRING("Test string", dest);
}

// makes no sense at all because strcpy_PF for example will still fetch strings with Adressvalue 0, it should be the bootloader
//void test_copyString_P_null_source(void) {}

void test_copyString_P_null_dest(void) {
    // Given // When
    flashHelper->copyString_P(NULL, addressOf(*PSTR("Test string")));
    // Then
    // No assertion needed as this should cause a segmentation fault
}

void test_copyString_P_empty_string(void) {
    // Given
    char dest[50];
    // When
    flashHelper->copyString_P(dest, addressOf(*PSTR("")));
    // Then
    TEST_ASSERT_NOT_NULL(dest);
    TEST_ASSERT_EQUAL_STRING("", dest);
}

void test_copyString_P_long_string(void) {
    // Given
    char dest[1024];
    // When
    flashHelper->copyString_P(dest, addressOf(loremIpsumProgMem1kB));
    // Then
    TEST_ASSERT_NOT_NULL(dest);
    TEST_ASSERT_EQUAL_STRING(loremIpsumRam1kB, dest);
}

#define BUFFERSIZE_FLASHHELPER_TEST (uint8_t) UINT8_MAX

void test_putString_P(void) {
    // Given
    stdoutCopyBuffer = calloc(BUFFERSIZE_FLASHHELPER_TEST, sizeof(char));
    stdout->put = mockPutSmallBufferCompare;
    // When
    flashHelper->putString_P(addressOf(*PSTR("Test string")));
    stdoutCopyBuffer[buffer_index] = '\0';
    // Then
    char* testString = "Test string";
    for (uint8_t i = 0; i < BUFFERSIZE_FLASHHELPER_TEST; i++) {
        if (i < buffer_index) {
            TEST_ASSERT_EQUAL_CHAR(testString[i], stdoutCopyBuffer[i]);
        } else {
            TEST_ASSERT_EQUAL_CHAR('\0', stdoutCopyBuffer[i]);
        }
    }
}

//void test_putString_P_null_string(void) {}

void test_putString_P_empty_string(void) {
    // Given
    stdoutCopyBuffer = calloc(BUFFERSIZE_FLASHHELPER_TEST, sizeof(char));
    stdout->put = mockPutSmallBufferCompare;
    // When
    flashHelper->putString_P(addressOf(*PSTR("")));
    stdoutCopyBuffer[buffer_index] = '\0';
    // Then
    char* testString = "";
    for (uint8_t i = 0; i < BUFFERSIZE_FLASHHELPER_TEST; i++) {
        if (i < buffer_index) {
            TEST_ASSERT_EQUAL_CHAR(testString[i], stdoutCopyBuffer[i]);
        } else {
            TEST_ASSERT_EQUAL_CHAR('\0', stdoutCopyBuffer[i]);
        }
    }
}

void test_putString_P_long_string(void) {
    // Given
    stdout->put = mockPutLoremIpsumCompare;
    farMemStringIndex = 0;
    farProgMemStringUnderInspektion = addressOf(loremIpsum9);
    // When
    flashHelper->putString_P(farProgMemStringUnderInspektion);
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

    setUpIndividual = individualStartUpFlashHelperTests;
    tearDownIndividual = individualTearDownFlashHelperTests;

    flashHelper = dOS_initFlashHelper(0);

    UNITY_BEGIN();
    RUN_TEST(test_createString_P);
//    RUN_TEST(test_createString_P_null_flashString);
    RUN_TEST(test_createString_P_null_helper);
    RUN_TEST(test_createString_P_empty_string);
    RUN_TEST(test_createString_P_long_string);


    RUN_TEST(test_copyString_P);
//    RUN_TEST(test_copyString_P_null_source);
    RUN_TEST(test_copyString_P_null_dest);
    RUN_TEST(test_copyString_P_empty_string);
    RUN_TEST(test_copyString_P_long_string);


    RUN_TEST(test_putString_P);
//    RUN_TEST(test_putString_P_null_string);
    RUN_TEST(test_putString_P_empty_string);
    RUN_TEST(test_putString_P_long_string);




    RUN_TEST(test_lengthOfString_P);
    RUN_TEST(test_readByte_P);
    RUN_TEST(test_compareString_P);
    RUN_TEST(test_createFileString_P);
    RUN_TEST(test_putFileString_P);
    RUN_TEST(test_getOrPutDosMessage);
    UNITY_END();

    free(flashHelper);
}