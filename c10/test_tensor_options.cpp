#include <vector>
#include <ATen/ATen.h>
#include <torch/torch.h>
#include <gtest/gtest.h>

#include "serialize.h"

/// TensorOptions有哪些属性呢
/// dtype, device, layout, memory_format, requires_grad_, pinned_memory
///


void check_tensor_options_equal(const at::TensorOptions& lhs, const at::TensorOptions& rhs) {
  std::cout << "============lhs TensorOptions============" << std::endl;
  std::cout << c10::toString(lhs) << std::endl;
  std::cout << "============lhs TensorOptions============" << std::endl;
  std::cout << "============rhs TensorOptions============" << std::endl;
  std::cout << c10::toString(rhs) << std::endl;
  std::cout << "============rhs TensorOptions============" << std::endl;

  EXPECT_EQ(c10::typeMetaToScalarType(lhs.dtype()), c10::typeMetaToScalarType(rhs.dtype()));
  EXPECT_EQ(lhs.device(), rhs.device());
  EXPECT_EQ(lhs.layout(), rhs.layout());
  EXPECT_EQ(lhs.memory_format_opt(), rhs.memory_format_opt());
  EXPECT_EQ(lhs.requires_grad_opt(), rhs.requires_grad_opt());
  EXPECT_EQ(lhs.pinned_memory_opt(), rhs.pinned_memory_opt());
}

TEST(TensorOptions, serialize) {
  c10::TensorOptions options;
  auto data = torch_test::serialize_to_vec_string(options);
  auto new_options = torch_test::restore(data, torch_test::TensorOptionsKind());
  check_tensor_options_equal(options, new_options);

  options = options.dtype(at::kByte);
  data = torch_test::serialize_to_vec_string(options);
  new_options = torch_test::restore(data, torch_test::TensorOptionsKind());
  check_tensor_options_equal(options, new_options);

  options = options.device(at::kCUDA);
  data = torch_test::serialize_to_vec_string(options);
  new_options = torch_test::restore(data, torch_test::TensorOptionsKind());
  check_tensor_options_equal(options, new_options);

  options = options.layout(at::kStrided);
  data = torch_test::serialize_to_vec_string(options);
  new_options = torch_test::restore(data, torch_test::TensorOptionsKind());
  check_tensor_options_equal(options, new_options);
  
  options = options.requires_grad(true);
  data = torch_test::serialize_to_vec_string(options);
  new_options = torch_test::restore(data, torch_test::TensorOptionsKind());
  check_tensor_options_equal(options, new_options);
  
  options = options.memory_format(c10::MemoryFormat::ChannelsLast);
  data = torch_test::serialize_to_vec_string(options);
  new_options = torch_test::restore(data, torch_test::TensorOptionsKind());
  check_tensor_options_equal(options, new_options);
  
  options = options.pinned_memory(true);
  data = torch_test::serialize_to_vec_string(options);
  new_options = torch_test::restore(data, torch_test::TensorOptionsKind());
  check_tensor_options_equal(options, new_options);
  
}
