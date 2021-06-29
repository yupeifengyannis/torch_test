#pragma once
#include <vector>
#include <string>

#include <c10/core/TensorOptions.h>
#include <c10/util/Exception.h>
#include <ATen/core/ivalue.h>
#include <torch/serialize.h>

namespace torch_test {

struct Kind {};
struct TensorOptionsKind : public Kind{};
struct DimnameKind : public  Kind{};
struct TensorKind : public Kind {};

template <typename Dtype>
std::vector<std::string> serialize_to_vec_string(const Dtype& data) {
  std::vector<std::string> ret_vec;
  auto vec = torch::pickle_save(torch::jit::IValue(data));
  ret_vec.push_back(std::string(vec.begin(), vec.end()));
  return ret_vec;
}

template <>
std::vector<std::string> serialize_to_vec_string<at::TensorOptions>(const at::TensorOptions& options) {
  std::vector<torch::jit::IValue> ivalue_vec;
  ivalue_vec.push_back(c10::typeMetaToScalarType(options.dtype()));
  ivalue_vec.push_back(options.device());
  ivalue_vec.push_back(options.layout());
  ivalue_vec.push_back(options.memory_format_opt());
  ivalue_vec.push_back(options.requires_grad_opt());
  ivalue_vec.push_back(options.pinned_memory_opt());
  
  std::vector<std::vector<char>> vec_vec;
  for(auto item : ivalue_vec) {
    vec_vec.push_back(torch::pickle_save(item));
  }
  std::vector<std::string> ret_vec;
  for(auto item : vec_vec) {
    ret_vec.push_back(std::string(item.begin(), item.end()));
  }
  return ret_vec;
}

at::TensorOptions restore(const std::vector<std::string>& str_vec, const TensorOptionsKind& kind) {
  std::vector<torch::jit::IValue> ivalue_vec;
  for(auto item : str_vec) {
    ivalue_vec.push_back(torch::pickle_load(std::vector<char>(item.begin(), item.end())));
  }
  at::TensorOptions options = at::TensorOptions() 
  // dtype
  .dtype(ivalue_vec[0].toScalarType())
  // device
  .device(ivalue_vec[1].toDevice())
  // layout
  .layout(ivalue_vec[2].toLayout())
  // memory_format
  .memory_format(ivalue_vec[3].toOptional<at::MemoryFormat>())
  // requred_grad
  .requires_grad(ivalue_vec[4].toOptional<bool>())
  // pinned_memory
  .pinned_memory(ivalue_vec[5].toOptional<bool>());
  
  return options;
}

at::Tensor restore(const std::vector<std::string>& str_vec, const TensorKind& kind) {
  TORCH_CHECK(str_vec.size() > 0, "input str vector is empty");
  torch::jit::IValue ivalue = torch::pickle_load(std::vector<char>(str_vec[0].begin(), str_vec[0].end()));
  return ivalue.toTensor();
}

}
