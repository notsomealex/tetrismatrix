# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(BLUEPAD32_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/utilities/bluepad32)

# To use BTstack from Pico SDK do
set(BTSTACK_ROOT ${PICO_SDK_PATH}/lib/btstack)

if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

project(tetrismatrix C CXX ASM)



pico_sdk_init()


include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/source)

# Needed when using BTstack from our branch
include_directories(${BTSTACK_ROOT}/3rd-party/bluedroid/encoder/include)
include_directories(${BTSTACK_ROOT}/3rd-party/bluedroid/decoder/include)
# Need for certain IDEs, like CLion. Otherwise it won't find btstack
include_directories(${BTSTACK_ROOT}/src)
add_subdirectory(${BLUEPAD32_ROOT}/src/components/bluepad32 libbluepad32)


include_directories(${CMAKE_SOURCE_DIR}/utilities/hub75)
add_subdirectory(utilities/hub75/libraries/pico_graphics)
add_subdirectory(utilities/hub75)
#add_subdirectory(utilities/pico-sqlite)

add_subdirectory(source)

#pico_enable_filesystem(tetrismatrix AUTO_INIT TRUE)

# optimizations to make binary size smaller.
target_compile_options(tetrismatrix PRIVATE -Os -Wno-error=incompatible-pointer-types)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
set(PICO_CXX_ENABLE_EXCEPTIONS 0)
set(PICO_CXX_ENABLE_RTTI 0)
set(CMAKE_BUILD_TYPE Release)
