#include <gtest/gtest.h>
#include "other.hpp"

TEST(TestOther, other)
{
    EXPECT_EQ("hello", foo());
}

