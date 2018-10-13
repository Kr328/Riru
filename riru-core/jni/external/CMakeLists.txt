cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -fvisibility=hidden")

file(GLOB XHOOK_SOURCE_FILES xhook/*.c)

add_library(xhook STATIC ${XHOOK_SOURCE_FILES})

target_link_libraries(xhook log)
