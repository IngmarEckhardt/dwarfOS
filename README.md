# DwarfOS

DwarfOS (dOS) is a lightweight "OS" designed for microcontrollers such as Arduino. It aims to minimize dependency on large
libraries, providing essential functionalities tailored for resource-constrained environments. This README provides an
overview of the features and components included in dOS.

## Features

### Custom Time Management

DwarfOS includes its own time management module (`mcu_clock.h`) for precise timekeeping. It supports real-time clock (
RTC) functionality when using a watch quartz.

### Serial Communication

Communication with external devices or host systems is facilitated through serial ports using the UART helper
module (`uart_helper.h`).

### Standard Input Buffer

The `input_queue.h` contains a circular buffer that dynamically adjusts its size. This buffer facilitates a `stdin` 
interface, enabling the utilization of standard functions such as `printf` and `fgets`. An illustrative example of its usage 
is provided in the `colossal-cave-adventure` repository.

### Lazy Loading Strings from Flash

Strings stored in flash memory can be lazily loaded into dynamic memory using the string repository
module (`string_repository.h`).

### UTC/CET/CEST Timestamps

DwarfOS provides support for generating timestamp strings in Coordinated Universal Time (UTC), Central European 
Time (CET), and Central European Summer Time (CEST).

### Integer to ASCII Conversion

A helper function is included for converting integers to ASCII strings, facilitating data conversion and output.

## Module Separation and Memory Management

One of the notable features of dOS is its modular architecture, allowing for separate loading and unloading of
modules.

Modules such as the string repository, UART helper, and time management are loaded into the heap memory, enabling
flexible memory management.

This approach allows users to load only the necessary modules, conserving memory resources on the microcontroller. 
Additionally, it facilitates easy modification; users can utilize function pointers to redirect to custom implementations, 
making it possible to mock individual functions in unit tests.

## Usage

The description of usage is provided in `main.c` and `setup.c` with an ample number of comments aimed at aiding beginners.
An additional example demonstrating how to include dOS into a project as a Git submodule, utilizing its own CMakeLists with 
library build options, is provided in the `Dwarf-Example` repository.

## Contributing

Contributions to improve or expand this library are welcome! If you find any issues or have suggestions for
enhancements, feel free to open an issue or submit a pull request.

## License

This library is licensed under the MIT License. See the `LICENSE` file for details.
