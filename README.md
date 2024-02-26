
# DwarfOS
DwarfOS is a lightweight "OS" designed for microcontrollers such as Arduino. It aims to minimize dependency on large libraries, providing essential functionalities tailored for resource-constrained environments. This README provides an overview of the features and components included in DwarfOS.

## Features

### Custom Time Management
DwarfOS includes its own time.h library for managing time-related functionalities. It supports real-time clock (RTC) functionality when using a watch quartz.

### Serial Communication
Communication with external devices or host systems is facilitated through serial ports.

### Lazy Loading Strings from Flash
Strings stored in flash memory can be lazily loaded into dynamic memory, allowing for efficient memory management.

### UTC/CET/CEST Timestamps
DwarfOS provides support for generating timestamp strings in various time zones, including Coordinated Universal Time (UTC), Central European Time (CET), and Central European Summer Time (CEST).

### Integer to ASCII Conversion
A helper function is included for converting integers to ASCII strings, facilitating data conversion and output.

## Contributing

Contributions to improve or expand this library are welcome! If you find any issues or have suggestions for enhancements, feel free to open an issue or submit a pull request.

## License

This library is licensed under the MIT License. See the `LICENSE` file for details.
