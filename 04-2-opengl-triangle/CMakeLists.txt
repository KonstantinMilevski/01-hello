cmake_minimum_required(VERSION 3.16)

project(04-2-opengl-triangle CXX C)

add_library(engine-04-2 SHARED engine.cxx
                               engine.hxx
                               glad/glad.h
                               glad/glad.c)

target_compile_features(engine-04-2 PUBLIC cxx_std_17)



find_package(sdl2 REQUIRED)
target_include_directories(engine-04-2 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-04-2 PRIVATE ${SDL2_LIBRARIES})

target_link_libraries(engine-04-2 PRIVATE
               -lSDL2
               -lGL
               )

add_executable(game-04-2 main.cxx)
target_compile_features(game-04-2 PUBLIC cxx_std_17)
target_link_libraries(game-04-2 PRIVATE engine-04-2)



