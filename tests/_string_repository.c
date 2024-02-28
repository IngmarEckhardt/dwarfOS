#include <unity.h>
#include <string_repository.h>
#include <malloc.h>

void setUp(void) {}
void tearDown(void) {}

void test_addString(void) {
    StringRepository * repositoryUnderTest;
    repositoryUnderTest = dOS_initStringRepository();
    // Create a LazyLoadingString object
    LazyLoadingString* lazyString = malloc(sizeof(LazyLoadingString));

    lazyString->flashString = (const char *) 0xff;
    lazyString->pointerToString = NULL; // Initialize pointerToString
    // Add the string to the repository
    LazyLoadingString** addedString = repositoryUnderTest->addString(lazyString);
    // Check if the string was added successfully
    TEST_ASSERT_NOT_NULL(addedString);
    TEST_ASSERT_EQUAL_PTR(lazyString, *addedString);
    free(repositoryUnderTest);
}

void test_getString(void) {
    StringRepository * stringRepository;
    stringRepository = dOS_initStringRepository();
    StringStorage * stringStorage = dOS_initStringStorage();
    // Create a LazyLoadingString object and add it to the repository
    LazyLoadingString* lazyString = malloc(sizeof(LazyLoadingString));
    lazyString->flashString = (const char *) 0xff;
    lazyString->pointerToString = NULL;
    stringRepository->addString(lazyString);
    // Try to retrieve the string
    char* retrievedString = stringRepository->getString(lazyString, stringStorage); // Pass NULL for StringStorage
    // Check if the retrieved string is not NULL
    TEST_ASSERT_NOT_NULL(retrievedString);
    // You might want to add further checks depending on your implementation
    free(stringRepository);
}

void test_freeString(void) {
    StringRepository * stringRepository;
    stringRepository = dOS_initStringRepository();
    // Create a LazyLoadingString object and add it to the repository
    LazyLoadingString* lazyString = malloc(sizeof(LazyLoadingString));
    lazyString->pointerToString = malloc(10 * sizeof(char)); // Allocate memory for pointerToString
    lazyString->flashString = (const char *) 0xff;
    stringRepository->addString(lazyString);
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
    stringRepository = dOS_initStringRepository();
    // Create a LazyLoadingString object and add it to the repository
    LazyLoadingString* lazyString = malloc(sizeof(LazyLoadingString));
    lazyString->flashString = (const char *) 0xff;
    lazyString->pointerToString = NULL;
    stringRepository->addString(lazyString);
    // Remove the string from management
    LazyLoadingString* removedString = stringRepository->removeStringFromManagement(lazyString);
    // Check if the removed string is the same as the original one
    TEST_ASSERT_EQUAL_PTR(lazyString, removedString);
    // Check if the string is no longer in the repository
    TEST_ASSERT_NULL(stringRepository->arrayOfManagedLazyStringPointers[0]);
    free(stringRepository);
}

void test_freeMemoryRandom(void) {
    // Initialize the repository
    StringRepository * repository = dOS_initStringRepository();
    TEST_ASSERT_NOT_NULL(repository);

    // Add some strings to the repository for testing
    for (int i = 0; i < MAX_SIZE_STRING_DB; i++) {
        LazyLoadingString* lazyString = malloc(sizeof(LazyLoadingString));
        lazyString->pointerToString = malloc(10 * sizeof(char)); // Allocate memory for pointerToString
        repository->arrayOfManagedLazyStringPointers[i] = lazyString;
        repository->arrayOfManagedLazyStringPointers[i]->flashString = (const char *) (i + 0xff);

    }

    // Free memory randomly with a percentage of 50%
    repository->freeMemoryRandom(50);

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

int main(void) {
    UNITY_BEGIN();
    // Run tests
    RUN_TEST(test_addString);
    RUN_TEST(test_getString);
    RUN_TEST(test_freeString);
    RUN_TEST(test_removeStringFromManagement);
    RUN_TEST(test_freeMemoryRandom);
    return UNITY_END();
}