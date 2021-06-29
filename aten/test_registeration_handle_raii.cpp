#include <vector>

#include <glog/logging.h>
#include <gtest/gtest.h>

#include <ATen/core/dispatch/RegistrationHandleRAII.h>

class Test1RegistrationHandleRAII : public ::testing::Test{};

void destroy_func1(){
  LOG(INFO) << "destroy func1";
}

void destroy_func2(){
  LOG(INFO) << "destroy func2";
}

TEST_F(Test1RegistrationHandleRAII, func){
  // 这种带有RAII名词的其实意思是离开作用域就销毁资源
  std::vector<c10::RegistrationHandleRAII> registras;
  registras.emplace_back(c10::RegistrationHandleRAII(destroy_func1));
  registras.emplace_back(c10::RegistrationHandleRAII(destroy_func2));
}
