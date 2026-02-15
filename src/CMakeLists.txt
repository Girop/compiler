target_sources(${MAIN_EXECUTABLE_NAME} PRIVATE main.cpp) # The only file not included in the project library
add_source(other.cpp)
add_subdirectory(ut)

