cmake_minimum_required(VERSION 3.20)
project(p4ioUser VERSION 0.0.1 LANGUAGES C)

set(CMAKE_C_FLAGS -m32)

set(BUILD_SHARED_LIBS OFF CACHE BOOL meow FORCE)
add_subdirectory(extern/libusb-cmake)

add_executable(p4iotest src/main.c)
target_link_libraries(p4iotest usb-1.0)

