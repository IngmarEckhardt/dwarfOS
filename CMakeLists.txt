cmake_minimum_required(VERSION 3.5)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_STANDARD 99)

# Ensure that you are using avr-gcc for this project. If not set previously, you can manually specify the path
# to the avr-gcc executable in this file. Additionally, you may need to correctly set the paths to avr-objcopy and
# avr-size for your environment at the end of the file.
# If you are using Atmel/Microchip Studio, create a new project and move these project files into this folder.
# Ensure that you include all the files and set "non-project" files to be visible.


# adjust to your MCU if needed
set(MCU atmega328p)

set(NAME "DwarfOS")
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)

project(${NAME} LANGUAGES C)
option(BUILD_DWARFOS_AS_LIBRARY "Build ${PROJECT_NAME} as a library" OFF)
option(DWARF_TIME "Build ${PROJECT_NAME} with own time.h/.c" ON)


set(CSTANDARD "-std=gnu99")
set(CWARN "-Wall -Wextra -Wstrict-prototypes -Wl,--gc-sections -Wl,--relax")
set(CTUNING "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections")
# use Optimization level -O2 if you have the space, it add several hundred bytes to program memory,
# but is optimized for more speed
set(COPT "-Os -lm ")
set(CMCU "-mmcu=${MCU}")
set(CFLAGS "${CMCU} ${COPT} ${CWARN} ${CSTANDARD} ${CTUNING}")
set(CMAKE_C_FLAGS "${CFLAGS}")

# Source Files
set(INCL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(SRC_FILES
        "${SRC_PATH}/ascii_helper.c"
        "${SRC_PATH}/heap_management_helper.c"
        "${SRC_PATH}/mcu_clock.c"
        "${SRC_PATH}/setup.c"
        "${SRC_PATH}/string_repository.c"
        "${SRC_PATH}/string_storage.c"
        "${SRC_PATH}/uart_helper.c"
)

set(HEADER_FILES
        "${INCL_PATH}/ascii_helper.h"
        "${INCL_PATH}/heap_management_helper.c"
        "${INCL_PATH}/mcu_clock.h"
        "${INCL_PATH}/setup.h"
        "${INCL_PATH}/string_repository.h"
        "${INCL_PATH}/string_storage.h"
        "${INCL_PATH}/uart_helper.h"
)

# generates the version.h in the include folder, containing the String with ProjectName and Version, as in version.h.in
configure_file(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h @ONLY)
include_directories(${INCL_PATH})

# Add time-related files if DWARF_TIME is enabled
if (DWARF_TIME)
    set(SRC_FILES ${SRC_FILES} "${SRC_PATH}/time.c")
    set(HEADER_FILES ${HEADER_FILES} "${INCL_PATH}/time.h")
    add_definitions(-D_TIME_T_DEFINED)
    add_definitions(-DTIME_H)
    add_definitions(-DDWARF_TIME)
endif ()

# Build as library if specified
if (BUILD_DWARFOS_AS_LIBRARY)
    add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${HEADER_FILES})
    target_include_directories(${PROJECT_NAME} PUBLIC
            ${INCL_PATH}
    )
else ()
    # Build as executable
    set(SRC_FILES ${SRC_FILES} "${SRC_PATH}/main.c")
    add_executable(${PROJECT_NAME} ${SRC_FILES})
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")
    # Toolchain path for AVR objcopy and size, change the path and file extensions to your environment, if you
    # have the path to the toolchain in the environment CMake can find it for you
    set(TOOLCHAIN_PATH "C:/Program Files (x86)/Atmel/Studio/7.0/toolchain/avr8/avr8-gnu-toolchain/bin/")
    set(AVR_OBJCOPY "${TOOLCHAIN_PATH}avr-objcopy.exe")
    set(AVR_SIZE "${TOOLCHAIN_PATH}avr-size.exe")

    add_custom_target(hex "${AVR_OBJCOPY}" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures
            "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex")
    add_custom_target(size "${AVR_SIZE}" "${PROJECT_NAME}.elf")

endif ()


set_directory_properties(PROPERTIES
        ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.hex;${PROJECT_NAME}.eeprom;${PROJECT_NAME}.lst")
