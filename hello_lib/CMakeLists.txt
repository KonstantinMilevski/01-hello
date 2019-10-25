cmake_minimum_required(VERSION 3.14)
project(hello_lib CXX)

set(SOURCE_LIB src/print.cpp  include/print.h)
set(STATIC_LIB hello-lib-static)
set(DYNAMIC_LIB hello-lib-dynamic)

# select STATIC library
add_library(${STATIC_LIB} STATIC ${SOURCE_LIB})
target_include_directories(${STATIC_LIB} PUBLIC
                          ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(${STATIC_LIB} PUBLIC cxx_std_17)

# select SHARED  library
add_library(${DYNAMIC_LIB} SHARED  ${SOURCE_LIB})
target_include_directories(${DYNAMIC_LIB} PUBLIC
                         ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(${DYNAMIC_LIB} PUBLIC cxx_std_17)

install(TARGETS ${STATIC_LIB} ${DYNAMIC_LIB}
    ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../tests
    LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../tests)

