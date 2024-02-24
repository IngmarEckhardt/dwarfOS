cmake_minimum_required(VERSION 3.5)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_STANDARD 99)
set(MCU atmega328p)

set(NAME "DwarfOS")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 9)
set(VERSION_PATCH 0)

project(${NAME} LANGUAGES C)
option(BUILD_DWARFOS_AS_LIBRARY "Build ${PROJECT_NAME} as a library" OFF)
option(DWARF_TIME "Build ${PROJECT_NAME} without own time.h/.c" ON)



set(CSTANDARD "-std=gnu99")
set(CWARN "-Wall -Wstrict-prototypes -Wl,--gc-sections -Wl,--relax")
set(CTUNING "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections")
#use Optimization level -O2 if you have the space, it add 600bytes to program memory, but is optimized for more speed
set(COPT "-Os -lm ")
set(CMCU "-mmcu=${MCU}")
set(CFLAGS "${CMCU} ${COPT} ${CWARN} ${CSTANDARD} ${CTUNING}")
set(CMAKE_C_FLAGS "${CFLAGS}")

# Source Files
set(INCL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(SRC_FILES
        "${SRC_PATH}/ascii_helper.c"
        "${SRC_PATH}/mcu_clock.c"
        "${SRC_PATH}/setup.c"
        "${SRC_PATH}/string_repository.c"
        "${SRC_PATH}/string_storage.c"
        "${SRC_PATH}/uart_helper.c"

)

if (!USE_STD_TIME_H)
    list(APPEND SRC_FILES ${SRC_PATH}/time.c)
endif ()

set(HEADER_FILES
        "${INCL_PATH}/ascii_helper.h"
        "${INCL_PATH}/mcu_clock.h"
        "${INCL_PATH}/setup.h"
        "${INCL_PATH}/string_repository.h"
        "${INCL_PATH}/string_storage.h"
        "${INCL_PATH}/uart_helper.h"
)

# generates the version.h in the include folder, containing the String with ProjectName and Version, as in version.h.in
configure_file(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h @ONLY)
include_directories(${INCL_PATH})

if(DWARF_TIME)
    set(SRC_FILES ${SRC_FILES} "${SRC_PATH}/time.c")
    set(HEADER_FILES ${HEADER_FILES} "${INCL_PATH}/time.h")
    add_definitions(-D_TIME_T_DEFINED)
    add_definitions(-DTIME_H)
    add_definitions(-DDWARF_TIME)
endif ()

if (BUILD_DWARFOS_AS_LIBRARY)
    add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${HEADER_FILES})
    target_include_directories(${PROJECT_NAME} PUBLIC
            ${INCL_PATH}
    )
else ()

    set(SRC_FILES ${SRC_FILES} "${SRC_PATH}/main.c")
    add_executable(${PROJECT_NAME} ${SRC_FILES})
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")

    set(TOOLCHAIN_PATH "C:/Program Files (x86)/Atmel/Studio/7.0/toolchain/avr8/avr8-gnu-toolchain/bin/")
    set(AVR_OBJCOPY "${TOOLCHAIN_PATH}avr-objcopy.exe")
    set(AVR_SIZE "${TOOLCHAIN_PATH}avr-size.exe")

    add_custom_target(hex "${AVR_OBJCOPY}" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures
            "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex")
    add_custom_target(size "${AVR_SIZE}" "${PROJECT_NAME}.elf")

endif ()


set_directory_properties(PROPERTIES
        ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.hex;${PROJECT_NAME}.eeprom;${PROJECT_NAME}.lst")
