#include <iostream>
#include <gtest/gtest.h>
#include <torch/torch.h>
#include <glog/logging.h>

class TestTorchOp : public ::testing::Test{};

// 直接测试add算子

TEST_F(TestTorchOp, add){
  torch::Tensor self = torch::ones(5);
  torch::Tensor other = torch::ones(5);
  c10::Scalar scalar(1);
  auto ret = at::add(self, other, scalar);
  auto data_ptr = ret.data_ptr<float>();
  for (int i = 0; i < ret.numel(); ++i){
    std::cout << data_ptr[i] << " ";
  }
  std::cout << std::endl;
}

TEST_F(TestTorchOp, tensor_save_load){
  torch::Tensor self = torch::ones(5);
  std::stringstream stream;
  torch::save(self, stream);
  std::cout << stream.str() << std::endl;
  torch::Tensor other;
  torch::load(other, stream);
}

TEST_F(TestTorchOp, add_cuda_cpu) {
  torch::Tensor self = torch::randn({3,5});
  torch::Tensor other = torch::randn({3,5});
  c10::Scalar scalar(1);
  auto ret_cpu = at::add(self, other, scalar);
  auto ret_cuda = at::add(self.to(at::kCUDA), other.to(at::kCUDA), scalar);
}




