cmake_minimum_required(VERSION 3.16)
project(06-3-m-move-tank CXX C)

add_library(engine-06-3 SHARED  engine.cxx
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

target_compile_features(engine-06-3 PUBLIC cxx_std_17)


find_package(OpenGL REQUIRED)
target_include_directories(engine-06-3 PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(engine-06-3 PRIVATE ${OPENGL_LIBRARIES})

find_package(sdl2 REQUIRED)
target_include_directories(engine-06-3 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-06-3 PRIVATE ${SDL2_LIBRARIES})


target_link_libraries(engine-06-3 PRIVATE
                                           -lSDL2
                                           -lGL
               )

add_executable(game-06-3 main.cxx)
target_compile_features(game-06-3 PUBLIC cxx_std_17)
target_link_libraries(game-06-3 PRIVATE engine-06-3)

install(TARGETS engine-06-3 game-06-3
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

