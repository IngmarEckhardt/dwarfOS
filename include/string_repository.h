#ifndef DWARFOS_STRING_REPOSITORY_H
#define DWARFOS_STRING_REPOSITORY_H

#include <stdint.h>
#include <string_storage.h>

#ifndef MAX_SIZE_STRING_DB
#define MAX_SIZE_STRING_DB 8 // Number of maximum Strings in DB, 2^n is recommended
#endif

typedef struct {
    /**
     * @brief Structure for managing lazy loading of strings from flash program memory
     *
     * FlashString holds the reference to the address of the string in the flash program memory, pointerToString holds a
     * reference to the heap address of the string if it was already loaded into the heap, or is NULL if it wasn't loaded
     * before.
     *
     * It's necessary to use at least one variable for a string, a pointer to a string, that you will use in your
     * program. This construct adds another pointer, so the use of RAM (stack of main) will increase by 2 bytes for a pointer at
     * avr-gcc compiler for every Lazy Loading String you manage here, but it will save space in RAM at startup for
     * every String that is larger than these 2 bytes.
     *
     * It only uses the RAM if the String is needed, and then uses the heap. You are free to free the heap memory for this
     * string (pointerToString) manually or add the string to the array and use rudimentary memory management that aims
     * for a low overhead of management.
     * */
    LazyLoadingString ** arrayOfManagedLazyStringPointers;

    /**
     * @brief Adds a string to the array of managed lazy loading strings.
     *
     * If you want to use the functionality to randomly or completely free the memory of the already loaded strings,
     * you can add them to the array of managed strings.
     *
     * @param stringToAdd The string to add.
     * @return An array of pointers to managed LazyLoadingString structures.
     */
    LazyLoadingString ** (* addString)(LazyLoadingString * stringToAdd, LazyLoadingString ** arrayOfManagedLazyStringPointers, uint8_t size);

    /**
    * @brief Retrieves a string from the repository.
    *
    * This function retrieves a string from the repository.
    *
    * @param stringToFetch The string to fetch.
    * @return A pointer to the fetched string.
    */
    char * (* getString)(LazyLoadingString * stringToFetch, StringStorage * stringStorage);

    /**
     * @brief Frees a string from the repository.
     *
     * This function frees a string from the repository.
     *
     * @param stringToKill The string to free.
     * @return A pointer to the freed LazyLoadingString structure.
     */
    LazyLoadingString * (* freeString)(LazyLoadingString * stringToKill);

    /**
     *
     * @brief Frees the memory of a percentage of the managed lazy strings.
     *
     * It uses the percentage to calculate how many strings in the RAM are freed, then frees() them and sets the pointer
     * back to NULL.
     *
     * @param percentage The percentage of memory to free.
     *
     */
    void (* freeMemoryRandom)(uint8_t percentage, LazyLoadingString ** arrayOfManagedLazyStringPointers, uint8_t size);

    /**
    * @brief Removes a string from management.
    *
    * This function removes a string from management.
    *
    * @param stringToKill The string to remove from management.
    * @return A pointer to the removed LazyLoadingString.
    */
    LazyLoadingString * (* removeStringFromManagement)(LazyLoadingString * stringToKill, LazyLoadingString ** arrayOfManagedLazyStringPointers, uint8_t size);
} StringRepository;

/**
 * @brief Initializes the string repository.
 *
 * This function initializes the string repository and returns a pointer to the StringRepository structure.
 * The string repository manages lazy loading, retrieval, and memory management of strings.
 *
 * To use the string repository, call this initialization function once in your program, typically at the beginning or whenever
 * string repository functionality is required.
 *
 * When string repository functionality is no longer needed or to conserve memory, you can unload the string repository module by
 * freeing the allocated memory using the standard `free()` function.
 * @note Make sure to check if the returned pointer is not NULL before using the functions.
 *
 * @return A pointer to the initialized StringRepository structure.
 */
StringRepository * dOS_initStringRepository(uint8_t size);

#endif /* DWARFOS_STRING_REPOSITORY_H */