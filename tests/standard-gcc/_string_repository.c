#include <unity.h>
#include "dwarf-os/string_repository.h"
#include <malloc.h>
#define SIZE_OF_INIT_STRING_REPO 8
void setUp(void) {}
void tearDown(void) {}

void test_addString(void) {
    StringRepository * repositoryUnderTest;
    repositoryUnderTest = dOS_initStringRepository(SIZE_OF_INIT_STRING_REPO);
    // Create a LazyLoadingString object
    LazyLoadingString* lazyString = malloc(sizeof(LazyLoadingString));

    lazyString->flashString = (const char *) 0xff;
    lazyString->pointerToString = NULL; // Initialize pointerToString
    // Add the string to the repository
    LazyLoadingString** addedString = repositoryUnderTest->addString(lazyString, repositoryUnderTest->lazyStringArray, SIZE_OF_INIT_STRING_REPO);
    // Check if the string was added successfully
    TEST_ASSERT_NOT_NULL(addedString);
    TEST_ASSERT_EQUAL_PTR(lazyString, *addedString);
    free(repositoryUnderTest);
}

void test_getString(void) {
    StringRepository * stringRepository;
    stringRepository = dOS_initStringRepository(SIZE_OF_INIT_STRING_REPO);
    FlashHelper * stringStorage = dOS_initFlashHelper(0);
    // Create a LazyLoadingString object and add it to the repository
    LazyLoadingString* lazyString = malloc(sizeof(LazyLoadingString));
    lazyString->flashString = (const char *) 0xff;
    lazyString->pointerToString = NULL;
    stringRepository->addString(lazyString, stringRepository->lazyStringArray, SIZE_OF_INIT_STRING_REPO);
    // Try to retrieve the string
    char* retrievedString = stringRepository->getString(lazyString, stringStorage); // Pass NULL for FlashHelper
    // Check if the retrieved string is not NULL
    TEST_ASSERT_NOT_NULL(retrievedString);
    // You might want to add further checks depending on your implementation
    free(stringRepository);
}

void test_freeString(void) {
    StringRepository * stringRepository;
    stringRepository = dOS_initStringRepository(SIZE_OF_INIT_STRING_REPO);
    // Create a LazyLoadingString object and add it to the repository
    LazyLoadingString* lazyString = malloc(sizeof(LazyLoadingString));
    lazyString->pointerToString = malloc(10 * sizeof(char)); // Allocate memory for pointerToString
    lazyString->flashString = (const char *) 0xff;
    stringRepository->addString(lazyString, stringRepository->lazyStringArray, SIZE_OF_INIT_STRING_REPO);
    // Free the string
    int8_t resultOfFreeString = stringRepository->freeString(lazyString);
    // Check if the freed string is the same as the original one
    TEST_ASSERT_EQUAL_INT8(0,resultOfFreeString);
    // Check if the pointerToString is NULL after freeing
    TEST_ASSERT_NULL(lazyString->pointerToString);
    free(stringRepository);
    free(lazyString);
}

void test_removeStringFromManagement(void) {
    StringRepository * stringRepository;
    stringRepository = dOS_initStringRepository(SIZE_OF_INIT_STRING_REPO);
    // Create a LazyLoadingString object and add it to the repository
    LazyLoadingString* lazyString = malloc(sizeof(LazyLoadingString));
    lazyString->flashString = (const char *) 0xff;
    lazyString->pointerToString = NULL;
    stringRepository->addString(lazyString, stringRepository->lazyStringArray, SIZE_OF_INIT_STRING_REPO);
    // Remove the string from management
    LazyLoadingString* removedString = stringRepository->removeFromManagement(lazyString, stringRepository->lazyStringArray, SIZE_OF_INIT_STRING_REPO);
    // Check if the removed string is the same as the original one
    TEST_ASSERT_EQUAL_PTR(lazyString, removedString);
    // Check if the string is no longer in the repository
    TEST_ASSERT_NULL(stringRepository->lazyStringArray[0]);
    free(stringRepository);
}

void test_freeMemoryRandom(void) {
    // Initialize the repository
    StringRepository * repository = dOS_initStringRepository(SIZE_OF_INIT_STRING_REPO);
    TEST_ASSERT_NOT_NULL(repository);

    // Add some strings to the repository for testing
    for (int i = 0; i < SIZE_OF_INIT_STRING_REPO; i++) {
        LazyLoadingString* lazyString = malloc(sizeof(LazyLoadingString));
        lazyString->pointerToString = malloc(10 * sizeof(char)); // Allocate memory for pointerToString
        repository->lazyStringArray[i] = lazyString;

        //ignore all pointer warning, they have 2 byte with 8bit avr-gcc, and this is just to get a value here.
        repository->lazyStringArray[i]->flashString = (const char *) (i + 0xff);

    }

    // Free memory randomly with a percentage of 50%
    repository->freeMemoryRandom(50, repository->lazyStringArray, SIZE_OF_INIT_STRING_REPO);

    // Check if approximately half of the strings are deallocated
    int deallocatedCount = 0;
    for (int i = 0; i < SIZE_OF_INIT_STRING_REPO; i++) {
        if (repository->lazyStringArray[i]->pointerToString == NULL) {
            deallocatedCount++;
        }
    }
    // Allow some tolerance due to rounding errors
    int expectedDeallocatedCount = SIZE_OF_INIT_STRING_REPO / 2;
    TEST_ASSERT_TRUE(abs(deallocatedCount - expectedDeallocatedCount) <= 1);

    // Free the remaining strings
    for (int i = 0; i < SIZE_OF_INIT_STRING_REPO; i++) {
        if (repository->lazyStringArray[i] != NULL) {
            free(repository->lazyStringArray[i]->pointerToString);
            free(repository->lazyStringArray[i]);
        }
    }
    free(repository);
}

void test_freeMemoryRandom_frees_at_least_one_element(void) {
    // Initialize the repository
    StringRepository * repository = dOS_initStringRepository(SIZE_OF_INIT_STRING_REPO);
    TEST_ASSERT_NOT_NULL(repository);

    // Add one string to the repository for testing
    LazyLoadingString* lazyString = malloc(sizeof(LazyLoadingString));
    lazyString->pointerToString = malloc(10 * sizeof(char)); // Allocate memory for pointerToString
    repository->lazyStringArray[SIZE_OF_INIT_STRING_REPO-2] = lazyString;

    // Free memory randomly with a percentage of 50%
    repository->freeMemoryRandom(1, repository->lazyStringArray, SIZE_OF_INIT_STRING_REPO);

    // Check if the string is deallocated
    TEST_ASSERT_NULL(repository->lazyStringArray[ (SIZE_OF_INIT_STRING_REPO-2)]->pointerToString);

    // Free the remaining strings
    free(repository->lazyStringArray[0]);
    free(repository);
}

void test_initManagedLazyLoadingStringArray(void) {
    const char * const flashStrings[] = {"string1", "string2", "string3"};
    uint8_t numStrings = sizeof(flashStrings) / sizeof(flashStrings[0]);

    LazyLoadingString ** result = initLazyStringArray(flashStrings, numStrings);

    // Check if the returned array is not NULL
    TEST_ASSERT_NOT_NULL(result);

    // Check each LazyLoadingString
    for (int i = 0; i < numStrings; i++) {
        LazyLoadingString * lazyString = result[i];

        // Check if the LazyLoadingString is not NULL
        TEST_ASSERT_NOT_NULL(lazyString);

        // Check if the flashString matches the corresponding string in the input array
        TEST_ASSERT_EQUAL_STRING(flashStrings[i], lazyString->flashString);

        // Check if pointerToString is NULL
        TEST_ASSERT_NULL(lazyString->pointerToString);
    }

    // Free the allocated memory
    for (int i = 0; i < numStrings; i++) {
        free(result[i]);
    }
    free(result);
}




int main(void) {
    UNITY_BEGIN();
    // Run tests
    RUN_TEST(test_addString);
    RUN_TEST(test_getString);
    RUN_TEST(test_freeString);
    RUN_TEST(test_removeStringFromManagement);
    RUN_TEST(test_freeMemoryRandom);
    RUN_TEST(test_freeMemoryRandom_frees_at_least_one_element);
    RUN_TEST(test_initManagedLazyLoadingStringArray);

    return UNITY_END();
}