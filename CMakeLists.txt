cmake_minimum_required(VERSION 3.0)

project(UBoat VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
    set(TARGET_BUILD_PLATFORM "linux")
elseif (APPLE)
    set(TARGET_BUILD_PLATFORM "osx")
elseif (WIN32)
    set(TARGET_BUILD_PLATFORM "win32")
endif()

message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "Target build platform is ${TARGET_BUILD_PLATFORM}")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/boat/Config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/boat/Config.h)

include_directories(boat)

file(GLOB COMMON_SRC ${PROJECT_SOURCE_DIR}/boat/*.cpp)
file(GLOB PLATFORM_SRC ${PROJECT_SOURCE_DIR}/os/${TARGET_BUILD_PLATFORM}/*.cpp)

set(SRC_FILES ${COMMON_SRC} ${PLATFORM_SRC})

add_executable(boat ${SRC_FILES})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG_")

message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
