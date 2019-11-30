cmake_minimum_required(VERSION 3.16)

project(05-1-vertex-morphing CXX C)

add_library(engine-05-1 SHARED engine.cxx
                               engine.hxx
                               glad/glad.h
                               glad/glad.c)

target_compile_features(engine-05-1 PUBLIC cxx_std_17)



find_package(sdl2 REQUIRED)
target_include_directories(engine-05-1 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-05-1 PRIVATE ${SDL2_LIBRARIES})

target_link_libraries(engine-05-1 PRIVATE
               -lSDL2
               -lGL
               )

add_executable(game-05-1 main.cxx)
target_compile_features(game-05-1 PUBLIC cxx_std_17)
target_link_libraries(game-05-1 PRIVATE engine-05-1)



