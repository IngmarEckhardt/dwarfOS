cmake_minimum_required(VERSION 3.27)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_STANDARD 99)
set(MCU atmega328p)

# Ensure that you are using avr-gcc for this project.
# Additionally, you may need to correctly set the paths to avr-objcopy and avr-size for your environment.
# If you are using Atmel/Microchip Studio, create a new project and move these project files into this folder.
# Ensure that you include all the files and set "non-project" files to be visible.

set(NAME "DwarfOS")
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 2)
project(${NAME} LANGUAGES C)

option(BUILD_DWARFOS_AS_LIBRARY "Build ${PROJECT_NAME} as a library" OFF)
option(DWARF_TIME "Build ${PROJECT_NAME} with own time.h/.c" ON)
option(DWARF_TEST "Build ${PROJECT_NAME} Tests with standard gcc" OFF)

# generates the version.h in the include folder, containing the String with ProjectName and Version, as in version.h.in
configure_file(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h @ONLY)

# Toolchain path for AVR objcopy and size, change the path and file extensions to your environment
set(TOOLCHAIN_PATH "C:/Program Files/JetBrains/CLion 2023.3.4/bin/avr8-gnu-toolchain-win32_x86_64/bin/")
set(AVR_OBJCOPY "${TOOLCHAIN_PATH}avr-objcopy.exe")
set(AVR_SIZE "${TOOLCHAIN_PATH}avr-size.exe")

# compiler flags

set(CSTANDARD "-std=gnu99")

if (DWARF_TEST)
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


set(INCL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(EXTERN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern")

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
include_directories(${INCL_PATH})

# Add time-related files if DWARF_TIME is enabled
if (DWARF_TIME)
    set(SRC_FILES ${SRC_FILES} "${SRC_PATH}/time.c")
    set(HEADER_FILES ${HEADER_FILES} "${INCL_PATH}/time.h")
    add_definitions(-D_TIME_T_DEFINED)
    add_definitions(-DDWARF_TIME)
endif ()

if (DWARF_TEST)
else ()
    if (BUILD_DWARFOS_AS_LIBRARY)
        add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${HEADER_FILES})
        target_include_directories(${PROJECT_NAME} PUBLIC ${INCL_PATH}
        )
    else ()
        set(SRC_FILES ${SRC_FILES} "${SRC_PATH}/main.c")
        add_executable(${PROJECT_NAME} ${SRC_FILES})
        set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")

        # build a hex file if needed
        add_custom_target(hex "${AVR_OBJCOPY}" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures
                "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex")
        # display size of solution
        add_custom_target(size "${AVR_SIZE}" "${PROJECT_NAME}.elf")
    endif ()
endif ()


if (DWARF_TEST)
    add_definitions(-DDWARFOS_TEST)
    # adjust to your system
    set(CMAKE_SYSTEM_NAME Windows)

    set(TEST_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tests")

    # build Unity
    add_subdirectory(${EXTERN_PATH}/Unity extern_build)

    # reduced libs for tests
    add_library(${PROJECT_NAME}Lib STATIC
            "${SRC_PATH}/ascii_helper.c" "${SRC_PATH}/time.c"
            "${INCL_PATH}/ascii_helper.h" "${INCL_PATH}/time.h"
    )
    target_include_directories(${PROJECT_NAME}Lib PUBLIC ${INCL_PATH})

    add_library(${PROJECT_NAME}LibReduced STATIC "${SRC_PATH}/string_repository.c" "${INCL_PATH}/string_repository.h")
    target_include_directories(${PROJECT_NAME}LibReduced PUBLIC "${TEST_PATH}/_include/_string_repo_test")


    add_executable(${PROJECT_NAME}_Time "${TEST_PATH}/_time.c")
    target_include_directories(${PROJECT_NAME}_Time PRIVATE ${INCL_PATH})
    target_link_libraries(${PROJECT_NAME}_Time ${PROJECT_NAME}Lib unity)


    add_executable(${PROJECT_NAME}_AsciiHelper "${TEST_PATH}/_ascii_helper.c")
    target_include_directories(${PROJECT_NAME}_AsciiHelper PRIVATE ${INCL_PATH})
    target_link_libraries(${PROJECT_NAME}_AsciiHelper ${PROJECT_NAME}Lib unity)


    add_executable(${PROJECT_NAME}_StringRepo "${TEST_PATH}/_string_repository.c" "${TEST_PATH}/_src/_string_storage.c")
    target_include_directories(${PROJECT_NAME}_StringRepo PRIVATE ${INCL_PATH})
    target_link_libraries(${PROJECT_NAME}_StringRepo ${PROJECT_NAME}LibReduced unity)

    include(CTest)
    add_test(Time ${PROJECT_NAME}_Time)
    add_test(AsciiHelper ${PROJECT_NAME}_AsciiHelper)
    add_test(StringRepo ${PROJECT_NAME}_StringRepo)
endif ()

set_directory_properties(PROPERTIES
        ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.hex;${PROJECT_NAME}.eeprom;${PROJECT_NAME}.lst")