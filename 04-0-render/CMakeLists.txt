cmake_minimum_required(VERSION 3.16)
project(render-test CXX)

add_executable(01-canvas         canvas.cxx
                                 canvas.hxx
                                 canvas_main.cxx
                                 )
target_compile_features(01-canvas PUBLIC cxx_std_17)

add_executable(02-line-render    line_render_main.cxx
                                 line_render.hxx
                                 line_render.cxx
                                 canvas.cxx
                                 )
target_compile_features(02-line-render PUBLIC cxx_std_17)

add_executable(03-line_triangle_render    line_triangle_render_main.cxx
                                          line_triangle_render.hxx
                                          line_triangle_render.cxx
                                          line_render.cxx
                                          canvas.cxx
                                 )
target_compile_features(03-line_triangle_render PUBLIC cxx_std_17)


add_executable(04-indexed_triangle_render   indexed_triangle_render_main.cxx
                                            indexed_triangle_render.hxx
                                            indexed_triangle_render.cxx
                                            line_triangle_render.cxx
                                            line_render.cxx
                                            canvas.cxx
                                 )
target_compile_features(04-indexed_triangle_render PUBLIC cxx_std_17)

add_executable(05-interpolated_triangle_render  interpolateded_triangle_render_main.cxx
                                                interpolateded_triangle_render.hxx
                                                interpolateded_triangle_render.cxx
                                                indexed_triangle_render.cxx
                                                line_triangle_render.cxx
                                                line_render.cxx
                                                canvas.cxx
                                 )

target_compile_features(05-interpolated_triangle_render PUBLIC cxx_std_17)

find_package(sdl2 REQUIRED)
message(STATUS "SDL2_LIBRARIES[${SDL2_LIBRARIES}] "
    "SDL2_INCLUDE_DIRS[${SDL2_INCLUDE_DIRS}]")


add_executable(06-render-windowed       render-windowed.cxx
                                        interpolateded_triangle_render.hxx
                                        interpolateded_triangle_render.cxx
                                        indexed_triangle_render.cxx
                                        line_triangle_render.cxx
                                        line_render.cxx
                                        canvas.cxx
                                          )
target_link_libraries(06-render-windowed PRIVATE ${SDL2_LIBRARIES})
target_include_directories(06-render-windowed PRIVATE ${SDL2_INCLUDE_DIRS})
if (MSVC)
    target_link_libraries(06-render-windowed PRIVATE SDL2::SDL2 SDL2::SDL2main)
endif()
target_compile_features(06-render-windowed PUBLIC cxx_std_17)
