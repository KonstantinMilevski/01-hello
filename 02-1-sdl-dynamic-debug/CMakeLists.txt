cmake_minimum_required(VERSION 3.16)

project(02-1-sdl-dynamic-debug CXX)

add_executable(02-1-sdl-dynamic-debug main.cxx)
target_compile_features(02-1-sdl-dynamic-debug PRIVATE cxx_std_17)
set(SDL2_INCLUDE_DIR /usr/local/include/SDL2/)
#set(SDL2_INCLUDE_DIR /home/MIk/tools/SDL2-2.0.10/include/)
set(SDL2_DEBUG_DIR /home/MIk/tools/SDL2-2.0.10/build-debug/)

find_library(SDL2-LIB-DEBUG
    NAMES libSDL2-2.0d.so
    PATHS ${SDL2_DEBUG_DIR}
    NO_DEFAULT_PATH)
message ("---------------------------")
message (${SDL2-LIB-DEBUG})

target_include_directories(02-1-sdl-dynamic-debug PRIVATE ${SDL2_INCLUDE_DIR})
target_link_libraries(02-1-sdl-dynamic-debug PRIVATE ${SDL2-LIB-DEBUG})

#/usr/local/include/SDL2
#-L/usr/local/lib -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -lSDL2
