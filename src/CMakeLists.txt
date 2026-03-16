target_sources(${MAIN_EXECUTABLE_NAME} PRIVATE main.cpp) # The only file not included in the project library
add_subdirectory(lexer)
add_subdirectory(ast)
add_subdirectory(codegen)
add_source(driver.cpp)
add_source(file.cpp)
add_source(loc.cpp)
