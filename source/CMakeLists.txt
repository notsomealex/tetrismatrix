
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20) 

# lik all source files.
add_executable(tetrismatrix main.cpp board.cpp game.cpp match.cpp piece.cpp input.cpp menu.cpp)


target_include_directories(tetrismatrix PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/utilities/hub75
        ${CMAKE_SOURCE_DIR}/utilities/hub75/libraries
)
# Generate PIO header
pico_generate_pio_header(tetrismatrix ${CMAKE_CURRENT_LIST_DIR}/../blink.pio)

# link all libraries.
target_link_libraries(tetrismatrix
        pico_stdlib
        hardware_spi
        hardware_i2c
        hardware_dma
        hardware_pio
        hardware_interp
        hardware_timer
        hardware_watchdog
        hardware_clocks
        hub75
)


pico_set_program_name(tetrismatrix "tetrismatrix")
pico_set_program_version(tetrismatrix "0.1")
pico_enable_stdio_uart(tetrismatrix 1)
pico_enable_stdio_usb(tetrismatrix 1)
pico_add_extra_outputs(tetrismatrix)
