# If you are using Atmel/Microchip Studio, create a new project and move the project files into this folder.
# Ensure that you include all the files and set "non-project" files to be visible.
cmake_minimum_required(VERSION 3.27)
set(CMAKE_C_STANDARD 99)
set(CMAKE_SYSTEM_NAME Windows)

set(NAME "DwarfOS-test")
set(VERSION_MAJOR 1)
set(VERSION_MINOR 3)
set(VERSION_PATCH 3)
project(${NAME} LANGUAGES C)

add_definitions(-DTEST_NON_AVR)

# compiler flags
set(CDEBUG "-g -ggdb --coverage")
set(CWARN "-Wall -Wextra -Wstrict-prototypes -Wl,--gc-sections -Wl,--relax")
set(CFLAGS "${CWARN} ${CDEBUG}")
set(CMAKE_C_FLAGS "${CFLAGS}")

# files and folders
set(INCL_PATH "../..")
set(INCL_SUBDIR "../../dwarf-os")
set(SRC_PATH "../../src")
set(EXTERN_PATH "../../extern")

include_directories(${INCL_PATH})

# get rid of time.h from standard gcc and the time_t definitions of host system
if (DWARF_TEST_TIME)
    add_definitions(-DTIME_H)
    add_definitions(-D_TIME_T_DEFINED)
endif ()

set(TEST_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
# build Unity
add_subdirectory(${EXTERN_PATH}/Unity extern_build)

# some reduced libs for tests
add_library(${PROJECT_NAME}Lib STATIC "${SRC_PATH}/time.c" "${INCL_SUBDIR}/time.h")
add_library(${PROJECT_NAME}LibReduced STATIC "${SRC_PATH}/string_repository.c" "${INCL_SUBDIR}/string_repository.h")
add_library(${PROJECT_NAME}LibInputQueueReduced STATIC "${SRC_PATH}/input_queue.c" "${INCL_SUBDIR}/input_queue.h")
add_library(${PROJECT_NAME}LibAsciiHelperReduced STATIC "${SRC_PATH}/ascii_helper.c" "${INCL_SUBDIR}/ascii_helper.h")

if (DWARF_TEST_TIME)
    add_executable(${PROJECT_NAME}_Time "${TEST_PATH}/_time.c")
    target_link_libraries(${PROJECT_NAME}_Time ${PROJECT_NAME}Lib ${PROJECT_NAME}LibAsciiHelperReduced unity)
else ()
    #input queue
    add_executable(${PROJECT_NAME}_inputQueue "${TEST_PATH}/_input_queue.c")
    target_link_libraries(${PROJECT_NAME}_inputQueue ${PROJECT_NAME}LibInputQueueReduced unity)

    # ascii_helper.c
    add_executable(${PROJECT_NAME}_AsciiHelper "${TEST_PATH}/_ascii_helper.c")
    target_link_libraries(${PROJECT_NAME}_AsciiHelper ${PROJECT_NAME}LibAsciiHelperReduced unity)

    # string_repository.c
    add_executable(${PROJECT_NAME}_StringRepo "${TEST_PATH}/_string_repository.c" "${TEST_PATH}/_src/mock_flash_helper.c")
    target_link_libraries(${PROJECT_NAME}_StringRepo ${PROJECT_NAME}LibReduced unity)

    include(CTest)
    add_test(AsciiHelper ${PROJECT_NAME}_AsciiHelper)
    add_test(StringRepo ${PROJECT_NAME}_StringRepo)
    add_test(InputQueue ${PROJECT_NAME}_inputQueue)
endif ()