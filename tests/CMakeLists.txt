cmake_minimum_required(VERSION 3.27)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_STANDARD 99)
if (NOT DEFINED MCU)
    set(MCU atmega328p)
endif ()

# If you are using Atmel/Microchip Studio, create a new project and move these project files into this folder.
# Ensure that you include all the files and set "non-project" files to be visible.

set(NAME "Dwarf-Mcu-Test")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 2)
project(${NAME} LANGUAGES C)

#Make sure you are using avr-gcc for this project. Add the folder containing avr-gcc to the PATH environment variable.
#Uncommenting this will work, but there are often better ways depending on your workflow/IDE.
# set(CMAKE_C_COMPILER avr-gcc)

# Toolchain path for AVR objcopy and size, change the path and file extensions to your environment
if (NOT DEFINED TOOLCHAIN_PATH)
    set(TOOLCHAIN_PATH "C:/Program Files/JetBrains/CLion 2023.3.4/bin/avr8-gnu-toolchain-win32_x86_64/bin/")
endif ()
if (NOT DEFINED FILE_EXT)
    set(FILE_EXT ".exe")
endif ()
set(AVR_OBJCOPY "${TOOLCHAIN_PATH}avr-objcopy${FILE_EXT}")
set(AVR_OBJDUMP "${TOOLCHAIN_PATH}avr-objdump${FILE_EXT}")
set(AVR_SIZE "${TOOLCHAIN_PATH}avr-size${FILE_EXT}")

# compiler flags
set(CSTANDARD "-std=gnu99")
set(CDEBUG "-g -ggdb")
set(CTUNING "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections")
# use Optimization level -O2 if you have the space, it uses more program memory but is optimized for more speed
set(COPT "-Os -lm -frename-registers")
set(CMCU "-mmcu=${MCU}")
set(CWARN "-Wall -Wextra -Wstrict-prototypes -Wl,--gc-sections -Wl,--relax")
set(CFLAGS "${CMCU} ${COPT} ${CWARN} ${CSTANDARD} ${CTUNING} ${CDEBUG}")
set(CMAKE_C_FLAGS "${CFLAGS}")

set(INCL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(EXTERN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../extern")

include_directories(${INCL_PATH})

# add Dwarf-OS
set(BUILD_DWARFOS_AS_LIBRARY ON CACHE BOOL "Build DwarfOS as a library")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. extern_dwarf)
add_subdirectory(${EXTERN_PATH}/Unity extern_unity)

set(STDIO ${PROJECT_NAME}_stdio)

add_executable(${STDIO} ${SRC_FILES} "${SRC_PATH}/_stdio.c")
target_include_directories(${STDIO} PUBLIC ${INCL_PATH})
target_link_libraries(${STDIO} DwarfOS unity)
set_target_properties(${STDIO} PROPERTIES OUTPUT_NAME "${STDIO}.elf")

add_custom_target(size "${AVR_SIZE}" "${STDIO}.elf")
add_custom_target(objdump "${AVR_OBJDUMP}" -h -s "${STDIO}.elf" > flash_memory_content.txt)





## build a hex file if needed
#add_custom_target(hex "${AVR_OBJCOPY}" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures
#        "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex")
## display size of solution
#add_custom_target(size "${AVR_SIZE}" "${PROJECT_NAME}.elf")
#
## show the complete content of the flash memory from the last version
#add_custom_target(objdump "${AVR_OBJDUMP}" -h -s "${PROJECT_NAME}.elf" > flash_memory_content.txt)

set_directory_properties(PROPERTIES
        ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.hex;${PROJECT_NAME}.eeprom;${PROJECT_NAME}.lst")