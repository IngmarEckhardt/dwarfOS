# If you are using Atmel/Microchip Studio, create a new project and move the project files into this folder.
# Ensure that you include all the files and set "non-project" files to be visible.
cmake_minimum_required(VERSION 3.27)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_STANDARD 99)

# define MCU before to avoid this fallback
if (NOT DEFINED MCU)
    set(MCU atmega328p)
endif ()

set(NAME "DwarfOS")
set(VERSION_MAJOR 1)
set(VERSION_MINOR 3)
set(VERSION_PATCH 1)
project(${NAME} LANGUAGES C)

option(BUILD_DWARFOS_AS_LIBRARY "Build ${PROJECT_NAME} as a library" OFF)
option(DWARF_WATCH_QUARTZ "Build ${PROJECT_NAME} with watch quartz" OFF)


#Make sure you are using avr-gcc for this project. Add the folder containing avr-gcc to the PATH environment variable and
#uncommenting this will work, but there are often better ways depending on your workflow/IDE.
# set(CMAKE_C_COMPILER avr-gcc)

if (DWARF_WATCH_QUARTZ)
    add_definitions(-DDWARFOS_WATCH_QUARTZ)
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
set(CDEBUG "-g -ggdb")
set(CTUNING "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections")
# use Optimization level -O2 if you have the space, it add several hundred bytes to program memory,
# but is optimized for more speed
set(COPT "-Os -lm -frename-registers")
set(CMCU "-mmcu=${MCU}")
set(CWARN "-Wall -Wextra -Wstrict-prototypes -Wl,--gc-sections -Wl,--relax")
set(CFLAGS "${CMCU} ${COPT} ${CWARN} ${CSTANDARD} ${CTUNING} ${CDEBUG}")
set(CMAKE_C_FLAGS "${CFLAGS}")

add_definitions(-D_TIME_T_DEFINED)

# files and folders
set(INCL_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(INCL_SUBDIR "${CMAKE_CURRENT_SOURCE_DIR}/dwarf-os")
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
        "${SRC_PATH}/stdio.c"
        "${SRC_PATH}/string_repository.c"
        "${SRC_PATH}/time.c"
        "${SRC_PATH}/uart_helper.c"
)
set(HEADER_FILES
        "${INCL_SUBDIR}/ascii_helper.h"
        "${INCL_SUBDIR}/flash_helper.h"
        "${INCL_SUBDIR}/heap_management_helper.h"
        "${INCL_SUBDIR}/input_queue.h"
        "${INCL_SUBDIR}/mcu_clock.h"
        "${INCL_SUBDIR}/pgm_textfile_generator.h"
        "${INCL_SUBDIR}/setup.h"
        "${INCL_SUBDIR}/stdio.h"
        "${INCL_SUBDIR}/string_repository.h"
        "${INCL_SUBDIR}/time.h"
        "${INCL_SUBDIR}/uart_helper.h"
)

include_directories(${INCL_PATH})

# targets


# generates the version.h in the include folder, containing the String with ProjectName and Version, as in version.h.in
configure_file(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/dwarf-os/version.h @ONLY)

if (BUILD_DWARFOS_AS_LIBRARY)
    add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${HEADER_FILES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${INCL_PATH}
    )
else ()
    # add main.c, produces a elf file that you can burn to a device or use in simavr
    set(SRC_FILES ${SRC_FILES} "${SRC_PATH}/main.c")
    add_executable(${PROJECT_NAME} ${SRC_FILES})
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")

    # build a hex file if needed
    add_custom_target(hex "${AVR_OBJCOPY}" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures
            "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex")

    # display size of solution
    add_custom_target(size "${AVR_SIZE}" "${PROJECT_NAME}.elf")

    # show the complete content of the flash memory
    add_custom_target(objdump "${AVR_OBJDUMP}" -h -s "${PROJECT_NAME}.elf" > flash_memory_content.txt)
endif ()


set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.hex;${PROJECT_NAME}.eeprom;${PROJECT_NAME}.lst")