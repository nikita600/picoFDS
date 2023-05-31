cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(picoFDS_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_subdirectory(../../picoSD/FatFs_SPI build)

add_executable(picoFDS
    main.c
    fds/ram_adapter.c
    fds.c
    sd_card.c
    hw_config.c
)

pico_generate_pio_header(picoFDS ${CMAKE_CURRENT_LIST_DIR}/fds/read_data.pio)

target_sources(picoFDS PRIVATE main.c)

target_link_libraries(picoFDS PRIVATE
    pico_stdlib
    pico_multicore
    hardware_pio
    hardware_pwm
    hardware_clocks
    FatFs_SPI
)

pico_enable_stdio_usb(picoFDS 0)
pico_enable_stdio_uart(picoFDS 1)
pico_add_extra_outputs(picoFDS)
