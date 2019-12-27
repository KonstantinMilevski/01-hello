cmake_minimum_required(VERSION 3.16)

project(tetris CXX C)

add_library(engine SHARED engine.cxx
                               engine.hxx
                               glad/glad.h
                               glad/glad.c
                               texture_gl_es20.cxx
                               texture_gl_es20.hxx
                               vertex_buffer_impl.cxx
                               vertex_buffer_impl.hxx
                               shader_gl_es20.cxx
                               shader_gl_es20.hxx
                               picopng.hxx)

target_compile_features(engine PUBLIC cxx_std_17)



find_package(sdl2 REQUIRED)
target_include_directories(engine PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine PRIVATE ${SDL2_LIBRARIES})

target_link_libraries(engine PRIVATE
               -lSDL2
               -lGL
               )

add_executable(game main.cxx)
target_compile_features(game PUBLIC cxx_std_17)
target_link_libraries(game PRIVATE engine)



