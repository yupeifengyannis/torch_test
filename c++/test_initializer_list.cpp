#include <initializer_list>
#include "gtest/gtest.h"

class TestInitializerList : public ::testing::Test{};

TEST_F(TestInitializerList, initialize){
    std::initializer_list<int> data{1,2,3,4};
    EXPECT_EQ(data.begin(), std::begin(data));
    EXPECT_EQ(data.end(), std::end(data));

    data = std::initializer_list<int>{};
    EXPECT_EQ(data.size(), 0);
}

