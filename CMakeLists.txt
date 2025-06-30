cmake_minimum_required(VERSION 3.2)
project(gbemu C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


file(GLOB SRC_FILES
     CONFIGURE_DEPENDS
     lib/*.c
)

add_library(gbemu_lib STATIC ${SRC_FILES})

target_include_directories(gbemu_lib
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(gbemu gbemu/main.c)
target_link_libraries(gbemu PRIVATE gbemu_lib)