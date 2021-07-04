#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <gtest/gtest.h>

using namespace std;

class TestCPPNewFeature : public ::testing::Test{};

/// C++演进历史
/// C++98 (1.0)
/// C++03 (TR1, Technical Report1, 主要是对c++98的一些修补)
/// C++11 (2.0时代开启, 引入了大量的新特性)
/// C++14 
/// C++17 
/// C++20
/// 本文会重点介绍C++11和C++14的新特性，同时C++17的部分特性已经在某一些比较新的开源项目中已经
/// 使用起来了，比如pytorch，会根据需求介绍一些新特性

/// C++2.0的新特性主要是分为语言和标准库两个部分的新特性
/// language new feature
/// 1. auto
/// 2. range-base loop
/// 3. intializer list
/// 4. lambda expression
/// 5. variadic templates
/// 6. move semantics
/// 7. ...

/// library new feature
/// 1. type_traits
/// 2. unorderd container(underlying implementation is a hash table)
/// 3. forward_list
/// 4. array
/// 5. tuple
/// 6. thread
/// 7. ...




/// 语言特性之:auto
/// 在C++11中你可以不需要显式的指定一个变量的类型的，而是可以直接直接制定其类型为auto，编译器在编译的
/// 时候会对auto关键字进行类型推导

double f() {
  return 1.0;
}
TEST_F(TestCPPNewFeature, auto_test) {
  auto i = 42; // 自动会推导为int数据类型
  auto d = f(); // 自动会推导为dobule数据类型
  /// 一般来说auto的使用会让C++在某种程度上和python语言接近了一步，因为在python的大多数使用过程中我们是不需要
  /// 指定一个对象的数据类型的，但是坏处也是非常明显的，如果工程比较大的情况，在python语言中我们可能经常会搞不
  /// 清楚某个对象的数据类型是啥。所以对于程序员来说auto关键字也不能乱用，一般的使用场景是对象对应的数据类型的名称
  /// 非常的长比如迭代器，或者说有些对象我们无法给出明确的数据类型，比如lambda函数
  std::vector<string> vec;
  auto pos = vec.begin();
  std::vector<string>::iterator start = vec.begin();
  EXPECT_EQ(pos, start);

  /// 或者说是lambda表达式，我们一般也不清楚lambda表达式的类型
  auto l = [](int x) -> bool{ return x > 0 ? true : false; };
  EXPECT_TRUE(l(10));
  EXPECT_FALSE(l(-10));
}

/// 语言特性之:uniform initialization


/// variadic template is an atomic bomb level feature in C++2.0
/// variadic template就是参数数量不定，数量类型不定的模板

void printX() {} /// 用于递归结束时

/// 下面这个printX函数的主要功能是递归打印任意个数不同数据类型的数据
template <typename T, typename... Args>
void printX(const T& first_arg, const Args&... args) {
  /// 这个函数接受一个参数first_arg和一包参数...args，这个一包参数的个数随意，同是其参数类型
  /// 也是随意的。
  std::cout << first_arg << std::endl;
  std::cout << "variadic parameters size is " << sizeof...(args) << std::endl;
  printX(args...);
}


/// 注意:
/// ...就是一个所谓的pack(包)
/// 用于template parameters,就是一个template parameters pack(模板参数包)
/// 用于function parameters types, 就是function parameters types pack(函数参数类型包)
/// 用于function parameters, 就是function parameters pack(函数参数包)

TEST_F(TestCPPNewFeature, variadic_template) {
  printX(7.5, "hello", std::bitset<16>(377), 42);
}
