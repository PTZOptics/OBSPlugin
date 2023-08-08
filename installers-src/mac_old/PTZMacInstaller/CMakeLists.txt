cmake_minimum_required(VERSION 3.23)
project(PTZMacInstaller C OBJCXX)

set(CMAKE_C_STANDARD 99)

add_executable(PTZMacInstaller install.mm main.m)
find_library(COCOA Cocoa)
find_library(SECURITY Security)
target_link_libraries(PTZMacInstaller ${COCOA} ${SECURITY})
