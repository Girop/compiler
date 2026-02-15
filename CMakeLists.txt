cmake_minimum_required(VERSION 3.28)
project(project_name)

set(MAIN_EXECUTABLE_NAME "MyProgram")
set(MAIN_LIB_NAME "${MAIN_EXECUTABLE_NAME}_lib")

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

include(cmake/fetch_dependency.cmake)
include(cmake/add_source.cmake)
include(cmake/define_test.cmake)

add_executable(${MAIN_EXECUTABLE_NAME})
add_library(${MAIN_LIB_NAME} STATIC)
target_compile_features(${MAIN_LIB_NAME} PUBLIC cxx_std_20 )
target_include_directories(${MAIN_LIB_NAME} PUBLIC src)
target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE ${MAIN_LIB_NAME})

add_subdirectory(src)

