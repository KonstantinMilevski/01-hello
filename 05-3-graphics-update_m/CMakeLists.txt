cmake_minimum_required(VERSION 3.16)
project(05-3-texture-m-loading CXX C)

add_library(engine-05-3 SHARED engine.cxx
                               engine.hxx
                               glad/glad.h
                               glad/glad.c
                               shader_gl_es20.hxx
                               shader_gl_es20.cxx
                               texture_gl_es20.hxx
                               texture_gl_es20.cxx
                               color.hxx
                               color.cxx
                               picopng.hxx)
target_compile_features(engine-05-3 PUBLIC cxx_std_17)

find_package(OpenGL REQUIRED)
target_include_directories(engine-05-3 PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(engine-05-3 PRIVATE ${OPENGL_LIBRARIES})


find_package(sdl2 REQUIRED)
target_include_directories(engine-05-3 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-05-3 PRIVATE ${SDL2_LIBRARIES})

target_link_libraries(engine-05-3 PRIVATE
               -lSDL2
               -lGL
               )


add_executable(game-05-3 main.cxx)
target_compile_features(game-05-3 PUBLIC cxx_std_17)
target_link_libraries(game-05-3 PRIVATE engine-05-3)




