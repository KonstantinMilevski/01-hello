cmake_minimum_required(VERSION 3.16)

project(03-3-sdl-loop-to-engine-dll LANGUAGES CXX)
#######simple executable
#add_executable(03-engine game.cxx engine.cxx engine.hxx)
#target_compile_features(03-engine PUBLIC cxx_std_17)
#find_package(sdl2 REQUIRED)
#target_include_directories(03-engine PRIVATE ${SDL2_INCLUDE_DIR})
#target_link_libraries(03-engine PRIVATE ${SDL2_LIBRARIES})
########
add_library(03-engine SHARED engine.cxx engine.hxx)
target_compile_features(03-engine PUBLIC cxx_std_17)

find_package(sdl2 REQUIRED)
target_include_directories(03-engine PRIVATE ${SDL2_INCLUDE_DIR})
target_link_libraries(03-engine PRIVATE ${SDL2_LIBRARIES})

add_executable(03-game game.cxx)
target_compile_features(03-game PUBLIC cxx_std_17)
target_link_libraries(03-game PRIVATE 03-engine)
