cmake_minimum_required(VERSION 3.5)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_STANDARD 99)

set(NAME "DwarfOS-Test")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 9)
set(VERSION_PATCH 0)

add_definitions(-D_TIME_T_DEFINED)
add_definitions(-DTIME_H)
add_definitions(-DDWARF_TIME)

project(${NAME} LANGUAGES C)
set(INCL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../src")
set(EXTERN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../extern")
add_subdirectory(${EXTERN_DIR} extern_build)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fprofile-arcs -ftest-coverage")

set(SRC_FILES
        "${SRC_PATH}/ascii_helper.c"
        "${SRC_PATH}/time.c"
)

set(HEADER_FILES
        "${INCL_PATH}/ascii_helper.h"
        "${INCL_PATH}/time.h"
)
add_library(${PROJECT_NAME}Lib STATIC
        ${SRC_FILES} ${HEADER_FILES}
)
target_include_directories(${PROJECT_NAME}Lib PUBLIC
        ${INCL_PATH}
)


add_executable(${PROJECT_NAME}_Time
        _time.c
)
target_include_directories(${PROJECT_NAME}_Time PRIVATE
        ${INCL_PATH}
)

target_link_libraries(${PROJECT_NAME}_Time
        ${PROJECT_NAME}Lib
        Unity
)


add_executable(${PROJECT_NAME}_AsciiHelper
        _ascii_helper.c
)
target_include_directories(${PROJECT_NAME}_AsciiHelper PRIVATE
        ${INCL_PATH}
)

target_link_libraries(${PROJECT_NAME}_AsciiHelper
        ${PROJECT_NAME}Lib
        Unity
)

add_library(${PROJECT_NAME}LibReduced STATIC
        "${SRC_PATH}/string_repository.c" "${INCL_PATH}/string_repository.h"
)
target_include_directories(${PROJECT_NAME}LibReduced PUBLIC
        "_include/_string_repo_test"
)


add_executable(${PROJECT_NAME}_StringRepo
        _string_repository.c _src/_string_repo_test/_string_storage.c
)
target_include_directories(${PROJECT_NAME}_StringRepo PRIVATE
        ${INCL_PATH}
)

target_link_libraries(${PROJECT_NAME}_StringRepo
        ${PROJECT_NAME}LibReduced
        Unity
)






include(CTest)
add_test(Time ${PROJECT_NAME}_Time)
add_test(AsciiHelper ${PROJECT_NAME}_AsciiHelper)
add_test(StringRepo ${PROJECT_NAME}_StringRepo)