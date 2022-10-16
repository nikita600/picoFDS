cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(picoFDS_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_subdirectory(../../picoSD/FatFs_SPI build)

add_executable(picoFDS
    main.c
    hw_config.c
)

target_link_libraries(picoFDS
    pico_stdlib
    FatFs_SPI
)

pico_enable_stdio_usb(picoFDS 1)
pico_enable_stdio_uart(picoFDS 1)
pico_add_extra_outputs(picoFDS)
