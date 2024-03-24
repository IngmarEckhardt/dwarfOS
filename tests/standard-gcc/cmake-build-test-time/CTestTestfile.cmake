# CMake generated Testfile for 
# Source directory: C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc
# Build directory: C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/cmake-build-test-time
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[AsciiHelper]=] "DwarfOS-test_AsciiHelper")
set_tests_properties([=[AsciiHelper]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;66;add_test;C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;0;")
add_test([=[TimeH]=] "DwarfOS-test_Time")
set_tests_properties([=[TimeH]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;67;add_test;C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;0;")
add_test([=[StringRepo]=] "DwarfOS-test_StringRepo")
set_tests_properties([=[StringRepo]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;68;add_test;C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;0;")
add_test([=[InputQueue]=] "DwarfOS-test_inputQueue")
set_tests_properties([=[InputQueue]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;69;add_test;C:/Users/eckha/AtmelStudio/DwarfOS/DwarfOS/tests/standard-gcc/CMakeLists.txt;0;")
subdirs("extern_build")
