cmake_minimum_required(VERSION 3.16)
project(04-0-render CXX)

add_executable(04-0-render 00_canvas.cxx
                                 00_canvas.hxx
                                 00_canvas_main.cxx
                                 )
target_compile_features(04-0-render PUBLIC cxx_std_17)


