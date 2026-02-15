include_guard()

include(GoogleTest)
enable_testing()

function(define_test TESTNAME FILENAME)
    add_executable(${TESTNAME} ${FILENAME})
    target_link_libraries(${TESTNAME} PRIVATE GTest::gtest_main ${MAIN_LIB_NAME})
    gtest_discover_tests(${TESTNAME})
endfunction()

