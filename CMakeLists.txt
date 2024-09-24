cmake_minimum_required(VERSION 3.28)
project(p4ioUser VERSION 0.0.1 LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_search_module(LIBUSB REQUIRED libusb-1.0)

add_executable(p4iotest src/main.c)
target_link_libraries(p4iotest LIBUSB)

