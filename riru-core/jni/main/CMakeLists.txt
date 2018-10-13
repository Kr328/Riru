cmake_minimum_required(VERSION 3.4.1)

file(GLOB RIRU_SOURCE_FILES src/*.cpp)

add_library(memtrack SHARED ${RIRU_SOURCE_FILES} )

target_link_libraries(memtrack log xhook dl)
