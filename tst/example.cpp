#include <gtest/gtest.h>

TEST(example, should_be_equal)
{
    int a = 1;
    int b = 1;

    EXPECT_EQ(a, b);
}

TEST(example, should_be_not_equal)
{
    int a = 1;
    int b = 2;

    EXPECT_NE(a, b);
}