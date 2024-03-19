#ifndef DWARFOS_UART_HELPER_H
#define DWARFOS_UART_HELPER_H

#include <stdint.h>

/**
 * @brief Structure for UART (Universal Asynchronous Receiver-Transmitter) helper functions.
 *
 * UartHelper provides functions for transmitting and receiving data via UART communication.
 * It includes functions for transmitting single characters, strings, and receiving lines of characters.
 * Additionally, it offers a function to send messages with timestamps when the time module is available.
 */
typedef struct {
    /**
     * @brief Transmits a single character via UART.
     *
     * This function transmits a single character via UART communication.
     *
     * @param byte The character to transmit.
     */
    void (* usartTransmitChar)(unsigned char byte);

    /**
     * @brief Transmits a null-terminated string via UART.
     *
     * This function transmits a null-terminated string via UART communication.
     *
     * @param str The string to transmit.
     */
    void (* usartTransmitString)(char * str);

    /**
     * @brief Receives a line of characters via UART.
     *
     * This function receives a line of characters via UART communication.
     *
     * @param buffer The buffer to store the received line.
     * @param bufferSize The size of the buffer.
     */
    void (* usartReceiveLine)(char * buffer, uint8_t bufferSize);

    /**
     * @brief Sends messages with timestamps via UART.
     *
     * This function sends messages along with timestamps via UART communication.
     * It concatenates multiple strings and adds a timestamp if the time module is available.
     *
     * @param amountOfStrings The number of strings to concatenate.
     * @param strings An array of pointers to strings.
     */
    void (* sendMsgWithTimestamp)(uint8_t amountOfStrings, char * strings[]);
} UartHelper;

/**
 * @brief Initializes the UART helper.
 *
 * This function initializes the UART helper and returns a pointer to the UartHelper structure.
 * The UART helper provides functions for UART communication, including transmitting and receiving data.
 *
 * To use the UART helper, call this initialization function once at the beginning of your program or whenever UART functionality is required.
 *
 * When you no longer need UART functionality or to conserve memory, you can unload the UART helper module by freeing the allocated memory.
 *
 * @note Make sure to check if the returned pointer is not NULL before using the functions.
 *
 * @return A pointer to the initialized UartHelper structure, or NULL if initialization fails.
 */
UartHelper * dOS_initUartHelper(void);


#endif /* DWARFOS_UART_HELPER_H */
