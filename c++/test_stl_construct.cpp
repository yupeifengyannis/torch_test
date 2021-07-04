#include <string>
#include <cstdlib>
#include <vector>
#include <bits/stl_construct.h>
#include "gtest/gtest.h"
#include "glog/logging.h"

struct Widget{
    std::string name_;
    int age_;
    ~Widget(){
        name_ = "";
        age_ = -1;
    }
};
class TestSTLConstruct : public ::testing::Test{};

TEST_F(TestSTLConstruct, constructAndDestroy){
    Widget* ptr = static_cast<Widget*>(malloc(sizeof(Widget)));
    // 调用_Construct函数相当于调用了placement new函数
    std::_Construct(ptr, Widget{"john", 10});
    LOG(INFO) << "widget name is " << ptr->name_ << " and age is " << ptr->age_;
    EXPECT_STREQ(ptr->name_.c_str(), "john");
    EXPECT_EQ(ptr->age_, 10);
    // 调用_Destroy函数仅仅析构了相关的内容，但是没有回收内存
    std::_Destroy(ptr);
    free(ptr);
}

TEST_F(TestSTLConstruct, destroyAux){
    std::vector<Widget> widget_vec;
    for(int i = 0; i < 10; ++i){
        std::string name = "john" + std::to_string(i);
        int age = i;
        widget_vec.push_back(Widget{name, age});
    }
    // 使用下面这个函数并不会正真的调用析构函数
    std::_Destroy_aux<true>::__destroy(widget_vec.begin(), widget_vec.end());   
    for(int i = 0; i < 10; ++i){
        EXPECT_EQ(widget_vec[i].age_, i);
    }

    std::_Destroy_aux<false>::__destroy(widget_vec.begin(), widget_vec.end());
    for(int i = 0; i < 10; ++i){
        EXPECT_EQ(widget_vec[i].name_, "");
        EXPECT_EQ(widget_vec[i].age_, -1);
    }
}

TEST_F(TestSTLConstruct, Destroy){
    // 在std中
    EXPECT_FALSE(__has_trivial_destructor(Widget));
    std::vector<int> int_vec;
    std::vector<Widget> widget_vec;
    for(int i = 0; i < 10; ++i){
        int_vec.push_back(i);
        std::string name = "john" + std::to_string(i);
        int age = i;
        widget_vec.push_back(Widget{name, age});
    }
    std::_Destroy(int_vec.begin(), int_vec.end());
    for(int i = 0; i < 10; ++i){
        EXPECT_EQ(int_vec[i], i);
    }
    std::_Destroy(widget_vec.begin(), widget_vec.end());
    for(int i = 0; i < 10; ++i){
        EXPECT_EQ(widget_vec[i].name_, "");
        EXPECT_EQ(widget_vec[i].age_, -1);
    }
}






