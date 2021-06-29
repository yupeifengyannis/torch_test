#include <vector>
#include <gtest/gtest.h>
#include <ATen/ATen.h>
#include <torch/torch.h>

TEST(TestSerialize, tensor) {
  at::Tensor src = torch::randn({4,5});
  // transfer Tensor to IValue
  torch::jit::IValue ivalue = src;
  // serialize IValue to string
  std::vector<char> vec = torch::pickle_save(ivalue);

  // deserialize string to IValue
  torch::jit::IValue new_ivalue = torch::pickle_load(vec);

  // transfer IValue to Tensor
  at::Tensor dst = new_ivalue.toTensor();

  ASSERT_TRUE(dst.equal(src));
}



