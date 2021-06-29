#include <gtest/gtest.h>
#include "c10/util/Optional.h"

/// pytorch c10目录下面的Optional模块主要是借鉴与boost库中的optional模块
/// 在c++17中已经将optional这个功能添加到标准库中了，c10目录下的optional
/// 和c++17标准库中的optional功能基本是一样的，我们可以借鉴c++17标准库的
/// optional来进行学习，具体学习资料可以参考：
/// https://en.cppreference.com/w/cpp/utility/optional

int get_data() {
  int data = 1;
  return data;
}

double get_double_data() {
  double data = 1;
  return data;
}

class Data{};

Data get_class_data() {
  return Data();
}


TEST(TestOptional, int) {
  if(c10::optional<int> ret = get_data()) {
    std::cout << "get data " << *ret << std::endl;
  }
  c10::optional<int> data = get_double_data();
  std::cout << "get double data " << *data << std::endl;
}
