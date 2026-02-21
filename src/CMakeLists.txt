target_sources(${MAIN_EXECUTABLE_NAME} PRIVATE main.cpp) # The only file not included in the project library
add_subdirectory(lexer)
add_source(ast.cpp)
add_source(driver.cpp)
add_source(file.cpp)
add_source(loc.cpp)
add_source(parser.cpp)
add_source(sema.cpp)
add_source(type.cpp)
