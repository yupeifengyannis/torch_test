#include <gtest/gtest.h>
#include <stdexcept>
#include "c10/util/Array.h"

// test file for c10/util/Array.h and c10/util/Array.cpp
// c10/util/Array.cpp其实就是c10/util/Array.h

class TestC10UtilArray : public ::testing::Test{};

TEST_F(TestC10UtilArray, test_construct){
  c10::guts::array<int, 10> array;
  array.fill(100);
  for(int i = 0; i < 10; ++i){
    EXPECT_EQ(array[i], 100);
  }
}

TEST_F(TestC10UtilArray, test_no_return){
  c10::guts::array<int, 10> array;
  array.fill(10);
  EXPECT_THROW(array.at(10), std::out_of_range); 
}

TEST_F(TestC10UtilArray, test_equal){
  c10::guts::array<int, 10> array1;
  c10::guts::array<int, 10> array2;
  for(int i = 0; i < 10; ++i){
    array1[i] = i;
  }
  for(int i = 0; i < 10; ++i){
    array2[i] = i;
  }
  if(array1 == array2){

  }
}

