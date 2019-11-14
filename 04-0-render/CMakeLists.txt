cmake_minimum_required(VERSION 3.16)
project(04-render CXX)

add_executable(04-0-render 00_canvas.cxx
                                 00_canvas.hxx
                                 00_canvas_main.cxx
                                 )
target_compile_features(04-0-render PUBLIC cxx_std_17)

add_executable(04-1-render-line 00_canvas.cxx
                                01_line_render.hxx
                                01_line_render.cxx
                                01_line_render_main.cxx
                                 )
target_compile_features(04-1-render-line PUBLIC cxx_std_17)

add_executable(04-2-render-triangle 00_canvas.cxx
                                    01_line_render.cxx
                                    02_triangle_render.cxx
                                    02_triangle_render.hxx
                                    02_triangle_render_main.cxx
                                 )
target_compile_features(04-2-render-triangle PUBLIC cxx_std_17)


add_executable(04-3-render-triangle-indexed     00_canvas.cxx
                                                01_line_render.cxx
                                                02_triangle_render.cxx
                                                03_triangle_render_indexed.cxx
                                                03_triangle_render_indexed.hxx
                                                03_triangle_render_indexed_main.cxx
                                 )
target_compile_features(04-3-render-triangle-indexed PUBLIC cxx_std_17)

add_executable(04-4-render-triangle-interpolated    00_canvas.cxx
                                                    01_line_render.cxx
                                                    02_triangle_render.cxx
                                                    03_triangle_render_indexed.cxx
                                                    04_triangle_render_interpolated.cxx
                                                    04_triangle_render_interpolated.hxx
                                                    04_triangle_render_interpolated_main.cxx)
target_compile_features(04-4-render-triangle-interpolated PUBLIC cxx_std_17)


find_package(sdl2 REQUIRED)
message(STATUS "SDL2_LIBRARIES[${SDL2_LIBRARIES}] "
    "SDL2_INCLUDE_DIRS[${SDL2_INCLUDE_DIRS}]")


add_executable(05-0-render-windowed 05-0-render-windowed.cxx
                                    00_canvas.cxx
                                    01_line_render.cxx
                                    02_triangle_render.cxx
                                    03_triangle_render_indexed.cxx
                                    04_triangle_render_interpolated.cxx
                                    04_triangle_render_interpolated.hxx
                                          )
target_link_libraries(05-0-render-windowed PRIVATE ${SDL2_LIBRARIES})
target_include_directories(05-0-render-windowed PRIVATE ${SDL2_INCLUDE_DIRS})
if (MSVC)
    target_link_libraries(05-0-render-windowed PRIVATE SDL2::SDL2 SDL2::SDL2main)
endif()
target_compile_features(05-0-render-windowed PUBLIC cxx_std_17)


#install(TARGETS 04-render
#        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
#        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
#        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
