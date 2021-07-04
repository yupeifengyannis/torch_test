#include <utility>
#include "gtest/gtest.h"
#include "glog/logging.h"

class TestPair : public ::testing::Test{};

TEST_F(TestPair, make_pair){
    std::pair<int, float> p(1,2);
}