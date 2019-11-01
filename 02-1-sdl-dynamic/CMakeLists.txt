cmake_minimum_required(VERSION 3.16)

project(02-1-sdl-dynamic CXX)

add_executable(02-1-sdl-dynamic main.cxx)
target_compile_features(02-1-sdl-dynamic PRIVATE cxx_std_17)

set(SDL2_INCLUDE_DIR /usr/local/include/SDL2/ )
set(SDL2_LIBRARY /usr/local/lib/ )


find_package(sdl2 REQUIRED)
message ("---------------------------")

message (${SDL2_INCLUDE_DIRS})
message (${SDL2_LIBRARIES})

message ("---------------------------")
target_include_directories(02-1-sdl-dynamic PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(02-1-sdl-dynamic PRIVATE ${SDL2_LIBRARIES})

#if (MINGW)
#    # find out what libraries are needed for dynamicaly linking with libSDL.so
#    # using mingw64 cross-compiler

#    #$> /usr/x86_64-w64-mingw32/sys-root/mingw/bin/sdl2-config --libs
#    #-L/usr/x86_64-w64-mingw32/sys-root/mingw/lib -lmingw32 -lSDL2main -lSDL2 -mwindows

#    target_link_libraries(02-1-sdl-dynamic PRIVATE
#               -lmingw32
#               -lSDL2main
#               -lSDL2
#               -mwindows
#               )
#elseif(MSVC)
#    target_link_libraries(02-1-sdl-dynamic PRIVATE SDL2::SDL2 SDL2::SDL2main)
#endif()

#install(TARGETS 02-1-sdl-dynamic
#        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
#        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
#        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
