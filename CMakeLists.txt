cmake_minimum_required(VERSION 3.5)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_STANDARD 99)
set(MCU atmega328p)

set(NAME "DwarfOS")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 8)
set(VERSION_PATCH 0)

project(${NAME} LANGUAGES C)

set(CSTANDARD "-std=gnu99")
set(CWARN "-Wall -Wstrict-prototypes -Wl,--gc-sections -Wl,--relax")
set(CTUNING "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections")
#use Optimization level -O2 if you have the space, it add 600bytes to program memory, but is optimized for more speed
set(COPT "-Os -lm ")
set(CMCU "-mmcu=${MCU}")
set(CFLAGS "${CMCU} ${COPT} ${CWARN} ${CSTANDARD} ${CTUNING}")
set(CMAKE_C_FLAGS "${CFLAGS}")


set(INCL_PATH "include")
set(SRC_PATH "src")

file(GLOB SRC_FILES
        "${SRC_PATH}/main.c"
        "${SRC_PATH}/**/*.c"
)

##disable conflicts with time.h
add_definitions(-D_TIME_T_DEFINED)
add_definitions(-DTIME_H)

# generates the version.h in the include folder, containing the String with ProjectName and Version, as in version.h.in
configure_file (version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h @ONLY)
include_directories(${INCL_PATH})

add_executable(${PROJECT_NAME} ${SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")

set(TOOLCHAIN_PATH "C:/Program Files (x86)/Atmel/Studio/7.0/toolchain/avr8/avr8-gnu-toolchain/bin/")
set(AVR_OBJCOPY "${TOOLCHAIN_PATH}avr-objcopy.exe")
set(AVR_SIZE "${TOOLCHAIN_PATH}avr-size.exe")

add_custom_target(hex "${AVR_OBJCOPY}" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures
        "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex")
add_custom_target(size "${AVR_SIZE}" "${PROJECT_NAME}.elf")

set_directory_properties(PROPERTIES
        ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.hex;${PROJECT_NAME}.eeprom;${PROJECT_NAME}.lst")