cmake_minimum_required(VERSION 3.16)

project(07-01-sound LANGUAGES CXX)
add_executable(07-01-sound main.cpp)

target_compile_features(07-01-sound PUBLIC cxx_std_20)

find_package(sdl2 REQUIRED)
target_include_directories(07-01-sound PRIVATE ${SDL2_INCLUDE_DIRS} )
target_link_libraries (07-01-sound PRIVATE ${SDL2_LIBRARIES} )




