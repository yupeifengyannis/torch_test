#include <glog/logging.h>
#include <map>
#include <string>

// 这个例子是摘抄caffe仓库代码tools/caffe.cpp的注册机制，通过全局变量的方式进行
// 注册，因为全局变量的是在main函数运行之前进行初始化的

using BrewFunction = int(*)();
using BrewMap = std::map<std::string, BrewFunction>;
BrewMap g_brew_map;

#define RegisterBrewFunction(func)              \
  namespace{                                    \
    class __Register_##func {                   \
      public:                                   \
         __Register_##func() {                  \
          g_brew_map[#func]  = &func;           \
        }                                       \
    };                                          \
    __Register_##func g_register_##func;        \
  }                                     

static BrewFunction GetBrewFunction(const std::string& name){
  if(g_brew_map.count(name)){
    return g_brew_map[name];
  }
  else{
    LOG(ERROR) << "Availabel caffe actions";
    for(auto item : g_brew_map){
      LOG(ERROR) << "\t" << item.first;
    }
    LOG(FATAL) << "Unknown action:" << name;
    return nullptr;
  }
}

int test(){
  LOG(INFO) << "invoking testing function";
}
RegisterBrewFunction(test); // 将test函数注册到全局哈希表g_brew_map中

int train(){
  LOG(INFO) << "invoking training function";
}
RegisterBrewFunction(train); // 将train函数注册到全局哈希表g_brew_map中

int genoff(){
  LOG(INFO) << "invoking genoff function";
}
RegisterBrewFunction(genoff); // 将genoff函数注册到全局哈希表g_brew_map中

int main(int argc, char** argv){
  FLAGS_alsologtostderr = 1;
  if(argc == 2){
    return GetBrewFunction(std::string(argv[1]))();
  }
  else{
    LOG(FATAL) << "argc must be 2";
  }
}
