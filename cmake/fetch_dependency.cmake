include_guard()

include(FetchContent)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Gtest specific hack

FetchContent_Declare(
    googletest
    GIT_REPOSITORY git@github.com:google/googletest.git
    GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)

