cmake_minimum_required(VERSION 3.16)
project(02-sdl-static)

add_executable(02-sdl-static main.cxx)
target_compile_features(02-sdl-static PUBLIC cxx_std_17)

set(SDL2_INCLUDE_DIR /usr/local/include/SDL2)
#set(SDL2_LIBRARY /home/MIk/tools/SDL2-2.0.10/build/)
message(STATUS "path to include directories [${SDL2_INCLUDE_DIR}]")
message ("-------------------------------------------------------")
target_include_directories(02-sdl-static PRIVATE ${SDL2_INCLUDE_DIR})
find_library(SDL2_LIB libSDL2.a)


if(NOT SDL2_LIB)
    message(FATAL_ERROR "Error: find_library(...) did not find libSDL2.a")
else()
    message(STATUS "path to static libSDL2.a [${SDL2_LIB}]")
    message ("-------------------------------------------------------")
endif()

target_link_libraries(02-sdl-static
           ${SDL2_LIB} # full path to libSDL2.a force to staticaly link with it
           # -Wl,--no-undefined # not compiled on MacOS
           -lm
           -ldl
           -lpthread
           -lrt
           )

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
