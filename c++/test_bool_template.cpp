#include "gtest/gtest.h"

class TestBoolTemplate : public ::testing::Test{};

template <bool TAG>
bool get_value(){
    return TAG;
}

TEST_F(TestBoolTemplate, testGetValue){
    EXPECT_TRUE(get_value<true>());
    EXPECT_FALSE(get_value<false>());
}