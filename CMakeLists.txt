cmake_minimum_required(VERSION 3.3)
project(hello CXX)
add_subdirectory(hello_lib)

set(BIN_STATIC hello-bin-static)
set(BIN_DYNAMIC hello-bin-dynamic)

#Static BIN
add_executable(${BIN_STATIC} src/main.cpp)
target_compile_features(${BIN_STATIC}  PRIVATE cxx_std_17)
target_link_libraries(${BIN_STATIC}  LINK_PUBLIC hello-lib-static)
target_link_options(${BIN_STATIC}  PRIVATE -static)

#Dinamic BIN
add_executable(${BIN_DYNAMIC} src/main.cpp)
target_compile_features(${BIN_DYNAMIC} PRIVATE cxx_std_17)
target_link_libraries(${BIN_DYNAMIC} LINK_PUBLIC hello-lib-dynamic)

install(TARGETS ${BIN_STATIC} ${BIN_DYNAMIC}
       RUNTIME DESTINATION bin
       LIBRARY DESTINATION bin
       ARCHIVE DESTINATION bin)

#enable_testing()
include(CTest)
add_test(NAME check_${PROJECT_NAME}
         COMMAND ${BIN_STATIC}
         )
