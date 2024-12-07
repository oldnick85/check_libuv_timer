cmake_minimum_required(VERSION 3.20.0)
project(check_libuv_timer)

add_executable(check_libuv_timer main.cpp)

find_path(LIBUV_INCLUDE_DIR NAMES uv.h)
find_library(LIBUV_LIBRARIES NAMES uv libuv)

target_include_directories(check_libuv_timer PRIVATE ${LIBUV_INCLUDE_DIR})
target_link_libraries(check_libuv_timer ${LIBUV_LIBRARIES})
