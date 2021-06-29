#include <gtest/gtest.h>
#include <ATen/ATen.h>
#include <c10/core/Scalar.h>
#include <ATen/ScalarOps.h>

TEST(TestScalar, normal_scalar) {
  /// Scalar其实就是一种0维度的Tensor，我们可以将一般的C++数据类型给转换成对应的
  /// Scalar数据类型，比如int, float, bool, complex
  /// 
  c10::Scalar scalar1 = 10;
  EXPECT_EQ(scalar1.to<int>(), 10);
  ASSERT_FALSE(scalar1.isFloatingPoint());
  ASSERT_TRUE(scalar1.isIntegral(false));
  
  bool data = false;
  c10::Scalar scalar2 = data;
  ASSERT_FALSE(scalar2.isIntegral(false));
  
  ASSERT_TRUE(scalar2.isIntegral(true));
  
  c10::Scalar scalar3 = 10.;
  ASSERT_TRUE(scalar3.isFloatingPoint());
  EXPECT_EQ(scalar3.to<int>(), 10);

}

TEST(TestScalarType, all_type) {
  // Byte
  at::Tensor t1 = at::randn({1, 2}).to(at::kByte);
  EXPECT_EQ(t1.scalar_type(), c10::ScalarType::Byte);

  // Char
  at::Tensor t2 = at::randn({2,3}).to(at::kChar);
  EXPECT_EQ(t2.scalar_type(), c10::ScalarType::Char);
  
  // Short
  at::Tensor t3 = at::randn({3,4}).to(at::kShort);
  EXPECT_EQ(t3.scalar_type(), c10::ScalarType::Short);
}

TEST(TestScalar, to_tensor) {
  // transfer scalar to tensor
  c10::Scalar s1 = 10;
  at::Tensor t1 = scalar_to_tensor(s1);
  EXPECT_EQ(t1.dim(), 0);
  at::Tensor t2 = at::scalar_tensor(s1, at::device(at::kCPU).dtype(at::kLong));
  ASSERT_TRUE(t1.equal(t2));
  /// where is at::device function define
  /// options is a TensorOptions
  auto options = at::device(at::kCPU);
  std::cout << at::toString(options) << std::endl;
}







