cmake_minimum_required(VERSION 3.2)
project(gbemu C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(SDL2 REQUIRED)


file(GLOB SRC_FILES
     CONFIGURE_DEPENDS
     lib/*.c
)

add_executable(gbemu gbemu/main.c)

add_library(gbemu_lib STATIC ${SRC_FILES})

target_include_directories(gbemu_lib
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include
)


target_include_directories(gbemu PRIVATE /usr/include/SDL2)

target_link_libraries(gbemu
                    PRIVATE
                        gbemu_lib
                        SDL2::SDL2
                        SDL2_ttf
)

