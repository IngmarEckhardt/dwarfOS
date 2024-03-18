# If you are using Atmel/Microchip Studio, create a new project and move the project files into this folder.
# Ensure that you include all the files and set "non-project" files to be visible.
cmake_minimum_required(VERSION 3.27)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_STANDARD 99)
if (NOT DEFINED MCU)
    set(MCU atmega328p)
endif ()


set(NAME "DwarfOS")
set(VERSION_MAJOR 1)
set(VERSION_MINOR 1)
set(VERSION_PATCH 2)
project(${NAME} LANGUAGES C)

option(BUILD_DWARFOS_AS_LIBRARY "Build ${PROJECT_NAME} as a library" OFF)
option(DWARF_WATCH_QUARTZ "Build ${PROJECT_NAME} with watch quartz" OFF)
option(DWARF_TEST "Build ${PROJECT_NAME} Tests with standard gcc" OFF)
option(DWARF_TEST_TIME "Build ${PROJECT_NAME} with standard gcc but without time_t defined" OFF)


#Make sure you are using avr-gcc for this project. Add the folder containing avr-gcc to the PATH environment variable.
#Uncommenting this will work, but there are often better ways depending on your workflow/IDE.
if (DWARF_TEST)
else ()
    # set(CMAKE_C_COMPILER avr-gcc)
endif ()


# Toolchain path for AVR objcopy and size, change the path and file extensions to your environment

if (NOT DEFINED TOOLCHAIN_PATH)
    set(TOOLCHAIN_PATH "C:/Program Files/JetBrains/CLion 2023.3.4/bin/avr8-gnu-toolchain-win32_x86_64/bin/")
endif ()
if (NOT DEFINED FILE_EXT)
    set(FILE_EXT ".exe")
endif ()
set(AVR_OBJCOPY "${TOOLCHAIN_PATH}avr-objcopy${FILE_EXT}")
set(AVR_OBJDUMP "${TOOLCHAIN_PATH}avr-objdump.exe")
set(AVR_SIZE "${TOOLCHAIN_PATH}avr-size${FILE_EXT}")



# compiler flags

set(CSTANDARD "-std=gnu99")
if (DWARF_TEST)
    set(CMAKE_SYSTEM_NAME Windows)
    set(CDEBUG "-g -ggdb --coverage")
else ()
    set(CDEBUG "-g -ggdb")
    set(CTUNING "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections")
    # use Optimization level -O2 if you have the space, it add several hundred bytes to program memory,
    # but is optimized for more speed
    set(COPT "-Os -lm -frename-registers")
    set(CMCU "-mmcu=${MCU}")
endif ()
set(CWARN "-Wall -Wextra -Wstrict-prototypes -Wl,--gc-sections -Wl,--relax")
set(CFLAGS "${CMCU} ${COPT} ${CWARN} ${CSTANDARD} ${CTUNING} ${CDEBUG}")
set(CMAKE_C_FLAGS "${CFLAGS}")


if (DWARF_WATCH_QUARTZ)
    add_definitions(-DDWARFOS_WATCH_QUARTZ)
endif ()


# files and folders

set(INCL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(EXTERN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern")
set(SRC_FILES
        "${SRC_PATH}/ascii_helper.c"
        "${SRC_PATH}/flash_helper.c"
        "${SRC_PATH}/heap_management_helper.c"
        "${SRC_PATH}/input_queue.c"
        "${SRC_PATH}/mcu_clock.c"
        "${SRC_PATH}/pgm_textfile_generator.c"
        "${SRC_PATH}/setup.c"
        "${SRC_PATH}/string_repository.c"
        "${SRC_PATH}/time.c"
        "${SRC_PATH}/uart_helper.c"
)
set(HEADER_FILES
        "${INCL_PATH}/ascii_helper.h"
        "${INCL_PATH}/flash_helper.h"
        "${INCL_PATH}/heap_management_helper.h"
        "${INCL_PATH}/input_queue.h"
        "${INCL_PATH}/mcu_clock.h"
        "${INCL_PATH}/pgm_textfile_generator.h"
        "${INCL_PATH}/setup.h"
        "${INCL_PATH}/string_repository.h"
        "${INCL_PATH}/time.h"
        "${INCL_PATH}/uart_helper.h"
)

include_directories(${INCL_PATH})


# targets

if (DWARF_TEST)
else ()

    # generates the version.h in the include folder, containing the String with ProjectName and Version, as in version.h.in
    configure_file(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h @ONLY)

    if (BUILD_DWARFOS_AS_LIBRARY)
        add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${HEADER_FILES})
        target_include_directories(${PROJECT_NAME} PUBLIC ${INCL_PATH}
        )
    else ()
        # add main.c
        set(SRC_FILES ${SRC_FILES} "${SRC_PATH}/main.c")
        add_executable(${PROJECT_NAME} ${SRC_FILES})
        set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")

        # build a hex file if needed
        add_custom_target(hex "${AVR_OBJCOPY}" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures
                "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex")

        # display size of solution
        add_custom_target(size "${AVR_SIZE}" "${PROJECT_NAME}.elf")
        # show the complete content of the flash memory
        add_custom_target(objdump "${AVR_OBJDUMP}" -h -s "${PROJECT_NAME}.elf" > objdump_output.txt)
    endif ()
endif ()


# test - targets

if (DWARF_TEST)

    add_definitions(-DDWARFOS_TEST)
    if (DWARF_TEST_TIME)
        add_definitions(-DTIME_H)
        add_definitions(-D_TIME_T_DEFINED)
    else ()
        add_definitions(-DDWARFOS_TIME_H)
    endif ()
    # adjust to your system

    set(TEST_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    # build Unity
    add_subdirectory(${EXTERN_PATH}/Unity extern_build)

    # some reduced libs for tests
    if (DWARF_TEST_TIME)
        add_library(${PROJECT_NAME}Lib STATIC "${SRC_PATH}/time.c" "${INCL_PATH}/time.h")
    endif ()
    add_library(${PROJECT_NAME}LibReduced STATIC "${SRC_PATH}/string_repository.c" "${INCL_PATH}/string_repository.h")
    add_library(${PROJECT_NAME}LibInputQueueReduced STATIC "${SRC_PATH}/input_queue.c" "${INCL_PATH}/input_queue.h")
    add_library(${PROJECT_NAME}LibAsciiHelperReduced STATIC "${SRC_PATH}/ascii_helper.c" "${INCL_PATH}/ascii_helper.h")

    if (DWARF_TEST_TIME)
        # time.c
        add_executable(${PROJECT_NAME}_Time "${TEST_PATH}/_time.c")
        target_link_libraries(${PROJECT_NAME}_Time ${PROJECT_NAME}Lib ${PROJECT_NAME}LibAsciiHelperReduced unity)
    else ()

        # time.c
        add_executable(${PROJECT_NAME}_inputQueue "${TEST_PATH}/_input_queue.c")
        target_link_libraries(${PROJECT_NAME}_inputQueue ${PROJECT_NAME}LibInputQueueReduced unity)


        # ascii_helper.c
        add_executable(${PROJECT_NAME}_AsciiHelper "${TEST_PATH}/_ascii_helper.c")
        target_link_libraries(${PROJECT_NAME}_AsciiHelper ${PROJECT_NAME}LibAsciiHelperReduced unity)

        # string_repository.c
        add_executable(${PROJECT_NAME}_StringRepo "${TEST_PATH}/_string_repository.c" "${TEST_PATH}/_src/_string_storage.c")
        target_link_libraries(${PROJECT_NAME}_StringRepo ${PROJECT_NAME}LibReduced unity)
    endif ()
    include(CTest)
    add_test(AsciiHelper ${PROJECT_NAME}_AsciiHelper)
    add_test(StringRepo ${PROJECT_NAME}_StringRepo)
    add_test(InputQueue ${PROJECT_NAME}_inputQueue)
endif ()

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.hex;${PROJECT_NAME}.eeprom;${PROJECT_NAME}.lst")