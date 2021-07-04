#include <set>
#include <vector>
#include <iterator>
#include "gtest/gtest.h"


class TestIterator : public ::testing::Test{};

TEST_F(TestIterator, testNext){
    std::vector<int> vec;
    for(int i = 0; i < 10; ++i){
        vec.push_back(i);
    }
    auto iter = vec.begin();
    auto next_iter = std::next(iter); 
    EXPECT_EQ(*next_iter, 1);

    std::set<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(1);
    auto tree_begin = tree.begin();
    auto tree_next = std::next(tree_begin);
    EXPECT_EQ(*tree_next, 5);
}