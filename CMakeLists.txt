# Generated then altered Cmake Pico project file

set(PROJECT ClariMIDI)

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/home/steve/pico/pico-sdk")

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(${PROJECT} C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(${PROJECT} 
  src/${PROJECT}.c
  src/breath.c
  src/display.c
  src/keys.c
  src/menu.c
  src/midi.c
  src/ssd1306.c
  src/usb_descriptors.c)

# pico_generate_pio_header(${PROJECT} ${CMAKE_CURRENT_LIST_DIR}/touch.pio)

pico_set_program_name(${PROJECT} "${PROJECT}")
pico_set_program_version(${PROJECT} "0.0.4")


# I could not get the UART to run stably alongside MIDI USB...
# target_compile_definitions(${PROJECT} PRIVATE
#   # PICO_DEFAULT_UART_BAUD_RATE=9600
# 	PICO_DEFAULT_UART_TX_PIN=16
# 	PICO_DEFAULT_UART_RX_PIN=17
# )
pico_enable_stdio_uart(${PROJECT} 0) # enable for UART stdio messages
pico_enable_stdio_usb(${PROJECT} 0)  # do not enable for this project!

# target_compile_options(${PROJECT} PRIVATE -Wall)

add_subdirectory(../pico-mpr121/lib mpr121)

# Add the standard library to the build
target_link_libraries(${PROJECT}
  hardware_adc
  hardware_i2c
  # pico_multicore
  pico-mpr121
  pico_stdlib
  tinyusb_device
  tinyusb_board
)

# Add the standard include files to the build
target_include_directories(${PROJECT} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

pico_add_extra_outputs(${PROJECT})

