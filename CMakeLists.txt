cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)

set(CMAKE_CXX_STANDARD 17)

project(nrf24_pico_communication)

pico_sdk_init()

# Build Pico_TX
add_executable(pico_tx
    src/pico_tx.cpp
    src/NRF24.cpp
)
target_include_directories(pico_tx PUBLIC inc)
target_link_libraries(pico_tx pico_stdlib hardware_spi)
pico_add_extra_outputs(pico_tx)

# Build Pico_RX
add_executable(pico_rx
    src/pico_rx.cpp
    src/NRF24.cpp
)
target_include_directories(pico_rx PUBLIC inc)
target_link_libraries(pico_rx pico_stdlib hardware_spi)
pico_add_extra_outputs(pico_rx)
