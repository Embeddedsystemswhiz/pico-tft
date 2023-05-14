cmake_minimum_required(VERSION 3.18)

# Include build functions from Pico SDK
include(external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ standards
project(tft_driver C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
# TODO remove once docker is fixed
SET (CMAKE_C_COMPILER_WORKS 1)
SET (CMAKE_CXX_COMPILER_WORKS 1)

#include build functions from Pico SDK
set(CMAKE_INSTALL_PREFIX ../install)


# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()
add_executable(tft_test
        src/main.cpp
        src/drivers/elegoo_tft.cpp
        src/libs/elegoo_gfx.cpp
        src/libs/print.cpp
        )

include_directories(${CMAKE_SOURCE_DIR})

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(tft_test pico_stdlib  m)
target_link_options(tft_test PRIVATE -Wl,--print-memory-usage)

target_compile_options(tft_test PRIVATE -g3 -O0)

pico_enable_stdio_usb(tft_test 1)
pico_enable_stdio_uart(tft_test 0)
# create map/bin/hex file etc.
pico_add_extra_outputs(tft_test)

install(TARGETS tft_test DESTINATION bin)