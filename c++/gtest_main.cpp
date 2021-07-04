#include <glog/logging.h>
#include "gtest/gtest.h"

namespace testing{
  class MemoryLeakDetector : public EmptyTestEventListener{
    public:
      virtual void OnTestStart(const TestInfo& test_info){
        LOG(INFO) << "testsuite name is " << test_info.test_case_name() << 
          " test case name is " << test_info.name(); 
      }
  };
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
  ::testing::UnitTest::GetInstance()->listeners().Append(new ::testing::MemoryLeakDetector());
	return RUN_ALL_TESTS();
}
