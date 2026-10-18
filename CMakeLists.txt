cmake_minimum_required(VERSION 3.16)

set(APP_VERSION_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/Data/AppVersion.h")
file(STRINGS "${APP_VERSION_HEADER}" APP_VERSION_DEFINE REGEX "^#define ULTIMATE_REMOTE_VERSION ")
string(REGEX MATCH "\"([0-9]+\\.[0-9]+(\\.[0-9]+)?)\"" APP_VERSION_MATCH "${APP_VERSION_DEFINE}")
if(NOT APP_VERSION_MATCH)
    message(FATAL_ERROR "Invalid application version in ${APP_VERSION_HEADER}")
endif()
set(ULTIMATE_REMOTE_VERSION "${CMAKE_MATCH_1}")
project(UltimateRemoteLinux VERSION "${ULTIMATE_REMOTE_VERSION}" LANGUAGES CXX)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${APP_VERSION_HEADER}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ULTIMATE_REMOTE_IR_TX_PIN "12" CACHE STRING "Default GPIO configured by gpio-ir-tx/pwm-ir-tx overlay")
option(ULTIMATE_REMOTE_STATIC "Build a self-contained Linux executable" OFF)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

list(FILTER APP_SOURCES EXCLUDE REGEX ".*/Platform/Arduino/.*\\.cpp$")

add_executable(ultimate-remote ${APP_SOURCES})

target_include_directories(ultimate-remote PRIVATE src)
target_compile_options(ultimate-remote PRIVATE -Wall -Wextra -Wpedantic)
target_compile_definitions(ultimate-remote PRIVATE
    ULTIMATE_REMOTE_LINUX=1
    ULTIMATE_REMOTE_IR_TX_PIN=${ULTIMATE_REMOTE_IR_TX_PIN}
)
target_compile_options(ultimate-remote PRIVATE -include Platform/Compatibility/ArduinoCompat.h)

if(ULTIMATE_REMOTE_STATIC)
    target_link_options(ultimate-remote PRIVATE -static)
endif()
