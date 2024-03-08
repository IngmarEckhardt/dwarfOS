#include <unity.h>
#include <string_repository.h>
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
    LazyLoadingString** addedString = repositoryUnderTest->addString(lazyString, repositoryUnderTest->arrayOfManagedLazyStringPointers, SIZE_OF_INIT_STRING_REPO);
    // Check if the string was added successfully
    TEST_ASSERT_NOT_NULL(addedString);
    TEST_ASSERT_EQUAL_PTR(lazyString, *addedString);
    free(repositoryUnderTest);
}

void test_getString(void) {
    StringRepository * stringRepository;
    stringRepository = dOS_initStringRepository(SIZE_OF_INIT_STRING_REPO);
    StringStorage * stringStorage = dOS_initStringStorage();
    // Create a LazyLoadingString object and add it to the repository
    LazyLoadingString* lazyString = malloc(sizeof(LazyLoadingString));
    lazyString->flashString = (const char *) 0xff;
    lazyString->pointerToString = NULL;
    stringRepository->addString(lazyString, stringRepository->arrayOfManagedLazyStringPointers, SIZE_OF_INIT_STRING_REPO);
    // Try to retrieve the string
    char* retrievedString = stringRepository->getStringFromRamElseLoadFromFlash(lazyString, stringStorage); // Pass NULL for StringStorage
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
    stringRepository->addString(lazyString, stringRepository->arrayOfManagedLazyStringPointers, SIZE_OF_INIT_STRING_REPO);
    // Free the string
    LazyLoadingString* freedString = stringRepository->freeString(lazyString);
    // Check if the freed string is the same as the original one
    TEST_ASSERT_EQUAL_PTR(lazyString, freedString);
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
    stringRepository->addString(lazyString, stringRepository->arrayOfManagedLazyStringPointers, SIZE_OF_INIT_STRING_REPO);
    // Remove the string from management
    LazyLoadingString* removedString = stringRepository->removeStringFromManagement(lazyString, stringRepository->arrayOfManagedLazyStringPointers, SIZE_OF_INIT_STRING_REPO);
    // Check if the removed string is the same as the original one
    TEST_ASSERT_EQUAL_PTR(lazyString, removedString);
    // Check if the string is no longer in the repository
    TEST_ASSERT_NULL(stringRepository->arrayOfManagedLazyStringPointers[0]);
    free(stringRepository);
}

void test_freeMemoryRandom(void) {
    // Initialize the repository
    StringRepository * repository = dOS_initStringRepository(SIZE_OF_INIT_STRING_REPO);
    TEST_ASSERT_NOT_NULL(repository);

    // Add some strings to the repository for testing
    for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
        LazyLoadingString* lazyString = malloc(sizeof(LazyLoadingString));
        lazyString->pointerToString = malloc(10 * sizeof(char)); // Allocate memory for pointerToString
        repository->arrayOfManagedLazyStringPointers[i] = lazyString;

        //ignore all pointer warning, they have 2 byte with 8bit avr-gcc, and this is just to get a value here.
        repository->arrayOfManagedLazyStringPointers[i]->flashString = (const char *) (i + 0xff);

    }

    // Free memory randomly with a percentage of 50%
    repository->freeMemoryRandom(50, repository->arrayOfManagedLazyStringPointers, SIZE_OF_INIT_STRING_REPO);

    // Check if approximately half of the strings are deallocated
    int deallocatedCount = 0;
    for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
        if (repository->arrayOfManagedLazyStringPointers[i]->pointerToString == NULL) {
            deallocatedCount++;
        }
    }
    // Allow some tolerance due to rounding errors
    int expectedDeallocatedCount = MAX_SIZE_STRING_DB / 2;
    TEST_ASSERT_TRUE(abs(deallocatedCount - expectedDeallocatedCount) <= 1);

    // Free the remaining strings
    for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
        if (repository->arrayOfManagedLazyStringPointers[i] != NULL) {
            free(repository->arrayOfManagedLazyStringPointers[i]->pointerToString);
            free(repository->arrayOfManagedLazyStringPointers[i]);
        }
    }
    free(repository);
}

void test_initManagedLazyLoadingStringArray(void) {
    const char * const flashStrings[] = {"string1", "string2", "string3"};
    uint8_t numStrings = sizeof(flashStrings) / sizeof(flashStrings[0]);

    LazyLoadingString ** result = initManagedLazyLoadingStringArray(flashStrings, numStrings);

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



void test_loadStringFromFile(void) {
#define NUMBER_OF_ENTRIES 1

    uint8_t indizes[1] = {0};
    ProgMemString memString = {
        .indexNumbers = indizes,
        .stringInProgramMem = "test"
    };

    TextFile file = {
            .entries = &memString,
            .amountOfEntries = NUMBER_OF_ENTRIES,
            .sizeOfIndexArray = 1,
            .maxLengthOfStrings = 5

    };
    StringRepository * repository = dOS_initStringRepository(0);
    StringStorage * storage = dOS_initStringStorage();

    // Test when the TextFile object is NULL
    char * result = repository->loadStringFromFile(NULL, storage, 0);
    TEST_ASSERT_NULL(result);

    // Test when the StringStorage object is NULL
    result =  repository->loadStringFromFile(&file, NULL, 0);
    TEST_ASSERT_NULL(result);

    // Test when the index is not found in the TextFile object
    result =  repository->loadStringFromFile(&file, storage, 249); // Assuming 999 is not a valid index
    TEST_ASSERT_NULL(result);

    // Test when the index is found in the TextFile object
    result = repository->loadStringFromFile(&file, storage, 0); // Assuming 0 is a valid index
    TEST_ASSERT_NOT_NULL(result);
    // Assuming the string at index 0 is "mock"
    TEST_ASSERT_EQUAL_STRING("test", result);

    // Clean up
    free(result);
    free(storage);
    // Free the TextFile object
}

int main(void) {
    UNITY_BEGIN();
    // Run tests
    RUN_TEST(test_addString);
    RUN_TEST(test_getString);
    RUN_TEST(test_freeString);
    RUN_TEST(test_removeStringFromManagement);
    RUN_TEST(test_freeMemoryRandom);
    RUN_TEST(test_initManagedLazyLoadingStringArray);
    RUN_TEST(test_loadStringFromFile);
    return UNITY_END();
}