# CMake generated Testfile for 
# Source directory: C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc
# Build directory: C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/cmake-build-tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[AsciiHelper]=] "DwarfOS-test_AsciiHelper")
set_tests_properties([=[AsciiHelper]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;62;add_test;C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;0;")
add_test([=[StringRepo]=] "DwarfOS-test_StringRepo")
set_tests_properties([=[StringRepo]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;63;add_test;C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;0;")
add_test([=[InputQueue]=] "DwarfOS-test_inputQueue")
set_tests_properties([=[InputQueue]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;64;add_test;C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;0;")
subdirs("extern_build")
