cmake_minimum_required(VERSION 3.16)
project(05-2-texture-loading-m CXX C)

add_library(engine-05-2 SHARED engine.cxx
                               engine.hxx
                               picopng.hxx
                               glad/glad.h
                               glad/glad.c)
target_compile_features(engine-05-2 PUBLIC cxx_std_17)

find_package(sdl2 REQUIRED)
target_include_directories(engine-05-2 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-05-2 PRIVATE ${SDL2_LIBRARIES})

#find_package(gl REQUIRED)
#target_include_directories(engine-05-2 PRIVATE ${GL_INCLUDE_DIRS})
#target_link_libraries(engine-05-2 PRIVATE ${GL_LIBRARIES})

target_link_libraries(engine-05-2 PRIVATE
               SDL2
               GL
               )

add_executable(game-05-2 main.cxx)
target_compile_features(game-05-2 PUBLIC cxx_std_17)

target_link_libraries(game-05-2 PRIVATE engine-05-2)




