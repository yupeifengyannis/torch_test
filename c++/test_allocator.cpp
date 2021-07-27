#include <vector>
#include <string>
#include <complex>
#include <iostream>
#include <memory>
#include <ext/malloc_allocator.h>
#include <ext/pool_allocator.h>
#include <cstdlib>

#include <malloc.h>

#include "gtest/gtest.h"
#include "glog/logging.h"
#include <limits.h>

class TestMemoryPrimitive : public ::testing::Test{};
// c++中内存分配的原语有四种:
// 1、malloc和free，是c函数，在c++中不可以被重载
// 2、new和delete，是c++表达式，在c++不可以被重载
// 3、::operator new()和::operator delete，c++函数，可以被重载
// 4、std::allocator<T>::allocate()和std::allocator<T>::deallocate()，c++标准库分配器

TEST_F(TestMemoryPrimitive, basic_usage){
  // 第一种
  void* p1 = malloc(512); // 申请512 bytes
  free(p1);
  // 第二种
  std::complex<int>* p2 = new std::complex<int>(1,2); // 分配了一个对象
  delete p2;
  // 第三种
  void* p3 = ::operator new(512); //申请512 bytes
  ::operator delete(p3);
  // 第四种
  int* p4 = std::allocator<int>().allocate(3, (int*)0); // 分配了3个int大小的内存
  std::allocator<int>().deallocate(p4, 3);
}

// new expression具体含义
// Complex<int>* pc = new Complex<int>(1,2); 该语义可以分为以下三步：
// 1、void* mem = operator new(sizeof(Complex<int>)); //调用operator new申请内存，传入需要申请内存的大小
// 2、pc = static_cast<Complex<int>*>(mem); // 指针转型
// 3、pc->Complex<int>::Complex<int>(1,2); // 调用Complex的构造函数进行初始化
// hint: 其实operator new和malloc的基本语义是一样的，向系统申请特定量的内存

// delete expression具体含义
// delete pc; delete语义可以分为以下两步骤
// pc->~Complex<int>(); // 调用析构函数进行析构
// operator delete(pc); // 调用operator delete函数进行内存回收，其实和free是一样的

// 注意！！！
// 上面new expression和delete expression中涉及到调用构造函数和析构函数，但是我们直接
// 在程序中调用构造函数和析构函数编译器会抱怨的。

// 关于array new和array delete的使用说明

// 问题：
// 1、如果分配内存使用了array new，回收内存使用了delete会发生什么事情
// 2、array new之后调用array delete的时候为什么可以知道需要析构几次？

class MyString{
  public:
    MyString(){
      LOG(INFO) << "invoking MyString default ctor";
      ptr_ = static_cast<char*>(malloc(1));
      *ptr_ = '\0';
    } 
    MyString(const char* ptr){
      LOG(INFO) << "invoking MyString user-defined ctor";
      if(ptr){
        ptr_ = static_cast<char*>(malloc(strlen(ptr) + 1));
        strcpy(ptr_, ptr);
      }
      else{
        ptr_ = static_cast<char*>(malloc(1));
        *ptr_ = '\0';
      }
    }
    MyString(const MyString& other){
      LOG(INFO) << "invoking copy ctor";
      if(other.ptr_){
        ptr_ = static_cast<char*>(malloc(strlen(other.ptr_) + 1));
        strcpy(ptr_, other.ptr_);
      }
      else{
        ptr_ = static_cast<char*>(malloc(1));
        *ptr_ = '\0';
      }
    }
    MyString& operator=(const MyString& other){
      if(other.ptr_ == ptr_){
        return *this;
      }
      free(ptr_);
      ptr_ = static_cast<char*>(malloc(strlen(other.ptr_) + 1));
      strcpy(ptr_, other.ptr_);
    }
    ~MyString(){
      LOG(INFO) << "invoking MyString dctor";
      if(ptr_){
        free(ptr_);
        ptr_ = nullptr;
      }
    }
    char* getData() const{
      return ptr_;
    }
  private:
    char* ptr_;
};

TEST_F(TestMemoryPrimitive, array_new_array_delete){
  std::complex<int>* pca = new std::complex<int>[3];
  // 会调用3次默认构造函数
  // 但是不能给每个complex对象赋予自己想赋予的处置
  // 我们可以调用placement new来对各个object进行初始化
  std::complex<int>* tmp = pca;
  for(int i = 0; i < 3; ++i){
    new(tmp++)std::complex<int>(i, i);
  }
  tmp = pca;
  for(int i = 0; i < 3; ++i){
    EXPECT_EQ(tmp->real(), i);
    EXPECT_EQ(tmp->imag(), i);
    tmp++;
  }
  delete[] pca; // 调用三次析构函数来进行析构，然后回收内存
  // delete pca; // 如果我们用delete语义来回收内存的话没啥问题，只会调用一次析构函数
  // 因为std::complex中没有指针指向其他内存区块，所以也不会导致相应的内存泄露的情况。

  MyString* psa = new MyString[3];
  MyString* str_tmp = psa;
  for(int i = 0; i < 3; ++i){
    new(str_tmp++)MyString("hello"); // 调用placement new进行重新构造
  }
  str_tmp = psa;
  for(int i = 0; i < 3; ++i){
    LOG(INFO) << psa[i].getData();
  }
  delete[] psa;
  // delete psa; // 如果这么调用会会导致程序崩溃，发生了内存泄露
}

// placement new, 前面已经提到了placement new了，其作用就是调用构造函数给一块分配好的
// 内存进行初始化操作，它没有进行内存分配操作，因此placement new没有对应的placement delete
// 比如以下这段代码：
TEST_F(TestMemoryPrimitive, placement_new){
  char* buf = new char[sizeof(std::complex<int>)];
  std::complex<int>* pc = new(buf)std::complex<int>(1,2); // 调用placement new操作
  delete pc; 
}
// 其中调用placement new编译器可以将其翻译为：
// void* mem = ::operator new(sizeof(std::complex<int>), buf); // 注意在这里会传入一个buf的指针
// pc = static_cast<std::complex<int>*>(mem); // 指针转型
// pc->std::complex<int>::complext<int>(1,2); // 调用构造函数进行内存初始化
// 我们可以看到第一个步骤不仅给operator new函数传入需要分配内存的大小，而且还传入一个指针，这里
// 可以查看<new>中对placement new的写法
// <new>
// 128  // Default placement versions of operator new.
// 129  inline void* operator new(std::size_t, void* __p) _GLIBCXX_USE_NOEXCEPT // placement new直接返回指针
// 130  { return __p;  }
// 131  inline void* operator new[](std::size_t, void* __p) _GLIBCXX_USE_NOEXCEPT
// 132  { return __p;  }
// 133
// 134  // Default placement versions of operator delete.
// 135  inline void operator delete  (void*, void*) _GLIBCXX_USE_NOEXCEPT { } // 看着有placement delete，但其实没啥用
// 136  inline void operator delete[](void*, void*) _GLIBCXX_USE_NOEXCEPT { }


// c++ 内存分配的过程
// 实例程序如下：
// {
//  Foo* p = new Foo(x);
//  ...
//
//  delete p;
// }
// 这个new expression和delete expression可以有两种调用路径：
//    1、如果Foo类有自己定义Foo::operator new(size_t)和Foo::operator delete(void*)函数的话会优先调用Foo类中函数
//    2、否则调用::operator new(size_t)和::operator delete(void*)函数，这两个函数是全局的函数
// 无论是全局的operator new和delete还是Foo自己定义的operator delete函数底层都会调用malloc和free函数，这两个函数
// 是glibc的函数，当然malloc和free是弱符号，我们其实可以写自己的malloc和free函数来进行内存调用实验，这个将会在后面
// 看到。
// 因此在这里就牵引出一个问题了，即operator new和operator delete是可以重载的
//
// 重载全局的::operator new和::operator delete函数，这个影响无远弗界，不推荐使用
// void *operator new(size_t size)
// {
//   std::cout << "::operator new is overrided" << std::endl;
//   return malloc(size);
// 
// }
// void operator delete(void *ptr)
// {
//   std::cout << "::operator delete is overrided" << std::endl;
//   free(ptr);
// 
// }
// void *operator new[](size_t size)
// {
//   std::cout << "::operator new[] is overrided" << std::endl;
//   return malloc(size);
// 
// }
// void operator delete[](void *ptr)
// {
//   std::cout << "::operator delete[] is overrided" << std::endl;
//   free(ptr);
// 
// }


// 重载类的operator new和operator delete函数

class Foo{
  public:
    Foo(){
      LOG(INFO) << "invoking Foo's ctor";
    }
    ~Foo(){
      LOG(INFO) << "invoking Foos' dctor";
    }
    static void* operator new(size_t size){
      LOG(INFO) << "invoking Foo's operator new and size is " << size;
      return malloc(size);
    }
    static void operator delete(void* ptr){
      LOG(INFO) << "invoking Foo's operator delete";
      free(ptr);
    }
    static void* operator new[](size_t size){
      LOG(INFO) << "invoking Foo's operator new[] and size is " << size;
      return malloc(size);
    }
    static void operator delete[](void* ptr){
      LOG(INFO) << "invoking Foo's operator delete[]";
      free(ptr);
    }
};

TEST_F(TestMemoryPrimitive, test_foo_new){
  Foo* ptr = new Foo();
  delete ptr;
  Foo* ptr1 = new Foo[3];
  delete[] ptr1;
  Foo* ptr2 = new Foo();
  LOG(INFO) << "======force invoking ::operator delete============";
  ::delete ptr2;
  LOG(INFO) << "======force invoking ::operator delete============";
  Foo* ptr3 = new Foo[4];
  LOG(INFO) << "======force invoking ::operator delete[]============";
  ::delete [] ptr3;
  LOG(INFO) << "======force invoking ::operator delete[]============";

  // 不知道大家有没有关注打印的log信息，Foo这个类的没有任何成员变量，因此
  // 编译器分配的内存大小应该是1 byte，我们也看到ptr分配的时候内存是1 byte，
  // 但是为啥分配ptr1的时候内存是11个bytes，分配ptr3的时候内存是12个bytes,
  // 如果是new []的expression的话会多分配一个8个字节的内存用于记录分配对象
  // 的个数，可以方便后面delete[]的时候调用对应次数的析构函数，但是这有个
  // 前提：Foo类成员变量必须要有析构函数
  //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //   |             这里8个字节的位置应该是记录着分配对象的个数       |
  //mem->+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //   |                          对象1                                |
  //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //   |                          对象2                                |
  //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //   |                          对象3                                |
  //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  ptr1 = new Foo[4];
  EXPECT_EQ(*reinterpret_cast<int*>(ptr1-8) , 4);
  delete[] ptr1;
}

class Foo1{
  public:
    static void* operator new[](size_t size){
      LOG(INFO) << "invoking Foo1's operator new[] and size is " << size;
      return malloc(size);
    }
    Foo1(){};
    virtual ~Foo1(){};
};

class Foo2{
  public:
    Foo2(){}
    ~Foo2(){}
    static void* operator new[](size_t size){
      LOG(INFO) << "invoking Foo2's opeartor new[] and size is " << size;
      return malloc(size);
    }
    int a;
};

class Foo3{
  public:
    Foo3(){}
    static void* operator new[](size_t size){
      LOG(INFO) << "invoking Foo3's operator new[] and size is " << size;
      return malloc(size);
    }
    char a;
};

class Foo4{
  public:
    Foo4(){}
    static void* operator new[](size_t size){
      LOG(INFO) << "invoking Foo4's operator new[] and size is " << size;
      return malloc(size);
    }
    std::string str;
};

TEST_F(TestMemoryPrimitive, test_array_new_foo){
  // Foo1和Foo的区别在于Foo1里面有一根虚指针指向虚函数表，因此delete的时候
  // 必须调用析构函数来释放这个虚表资源
  Foo1* ptr = new Foo1[3];
  EXPECT_EQ(*reinterpret_cast<int*>(reinterpret_cast<char*>(ptr) - 8), 3);
  delete[] ptr;
  ptr = ::new Foo1[3]; // 调用::operator new函数
  EXPECT_EQ(*reinterpret_cast<int*>(reinterpret_cast<char*>(ptr) - 8), 3);
  delete[] ptr;

  // Foo2类中只有一个成员变量int因此每个对象的大小为4 
  Foo2* ptr1 = new Foo2[4];
  EXPECT_EQ(*reinterpret_cast<int*>(reinterpret_cast<char*>(ptr1) - 8), 4);
  delete[] ptr1;

  // Foo3类中只有一个成员变量char，因此大小对象的大小为1，但是没有自己定义的析构函数
  // 所有只分配了5 bytes内存
  Foo3* ptr2 = new Foo3[5];
  // EXPECT_NE(*reinterpret_cast<int*>(reinterpret_cast<char*>(ptr2) - 8), 5); //dangerous
  delete[] ptr2;

  // Foo4类中有一个string成员变量，但是没有写自己的析构函数，其实delete的时候必须要
  // 调用析构函数然后析构掉string成员变量，因为string类中有指针
  Foo4* ptr3 = new Foo4[6];
  EXPECT_EQ(*reinterpret_cast<int*>(reinterpret_cast<char*>(ptr3) - 8), 6);
  delete[] ptr3;

  std::string* ps = new std::string[4];
  EXPECT_EQ(*reinterpret_cast<int*>(reinterpret_cast<char*>(ps) - 8), 4);
  delete[] ps;

  // 其实这个分配记录对象个数内存是由编译器说了算的，我这里只能给出基本的现象，但是不能
  // 给出结论，只是再次提醒大家new []之后需要调用delete [],加深一点印象
}

// 如何禁止对象产生在heap上？ 参考《More Effective C++》第145页
// 禁止对象产生在heap上我们只要让 new Widget()不能被编译通过，那么可以这么干
// 将Widget operator new和operator delete放到private中
class Widget{
  public:
    Widget(){}
  private:
    static void* operator new(size_t size){
      return malloc(size);
    }
    static void operator delete(void* ptr){
      free(ptr);
    }
    static void* operator new[](size_t size){
      return malloc(size);
    }
    static void operator delete[](void* ptr){
      free(ptr);
    }
};

TEST_F(TestMemoryPrimitive, forbid_new){
  // Widget* ptr = new Widget(); 这个编译就会挂，因为operator new为私有函数
  Widget* ptr = ::new Widget(); // 使用全局的operator new，但是一般人不会这么用
  ::delete ptr;
  // Widget* ptr1 = new Widget[3]; // 这个编译会挂，因为operator new[]为私有函数
  Widget* ptr1 = ::new Widget[3];
  ::delete[] ptr1;
}


// 开始介绍内存池的想法,一般来说我们通过malloc函数(operator new和operator new[]
// 底层都是调用malloc函数)分佩内存的时候，其实从系统中获取得到的内存不仅仅是我们
// 传入size的大小，因为malloc底层还需要额外分配内存记录一些信息，比如分配内块大小的信息
// 等等，这部分内容涉及到glibc的malloc，会在本文最后去分析，我们只是简单的看一下
// malloc分配的内存是长什么样子的。
//
// chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//         |             Size of previous chunk, if allocated            | |
//         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//         |             Size of chunk, in bytes                       |M|P|
// mem->   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//         |             User data starts here...                          .
//         .                                                               .
//         .             (malloc_usable_size() bytes)                      .
//         .                                                               |
// nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//         |             Size of chunk                                     |
//         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class TestStdAllocator : public ::testing::Test{};
TEST_F(TestStdAllocator, rawMalloc){
  // 在这里测试例子中我们分配11个double类型的数据，但是发现他们的指针的距离并不是
  // 我们想象的8bytes，而是最小是32bytes。也可以从侧面证明malloc有相应的内存开销
  std::vector<double*> ptr_vec(11); 
  double* pre_ptr = static_cast<double*>(malloc(sizeof(double)));
  ptr_vec.push_back(pre_ptr);
  for(int i = 0; i < 10; ++i){
    double* ptr = static_cast<double*>(malloc(sizeof(double)));
    LOG(INFO) << "ptr memory address" << ptr;
    LOG(INFO) << "address distance is " << (ptr - pre_ptr) * sizeof(double) << " [byte]";
    pre_ptr = ptr;
    ptr_vec.push_back(pre_ptr);
  }
  for(auto ptr : ptr_vec){
    free(ptr);
  }
}

TEST_F(TestStdAllocator, rawNew){
  // 对于new表达式也是一样的，指针的距离最小也是32bytes。
  std::vector<double*> ptr_vec(11);
  double* pre_ptr = new double(1);
  ptr_vec.push_back(pre_ptr);
  for(int i = 0; i < 10; ++i){
    double* ptr = new double(1);
    LOG(INFO) << "ptr memory address" << ptr;
    LOG(INFO) << "address distance is " << (ptr - pre_ptr) * sizeof(double) << " [byte]";
    pre_ptr = ptr;
    ptr_vec.push_back(pre_ptr);
  }
}

TEST_F(TestStdAllocator, poolAllocator){
  // 这是c++ stl中的pool allocator，使用内存池分配11个double对象，我们可以看到每个
  // 对象指针的距离和sizeof(double)一样。
  auto pool_allocator = __gnu_cxx::__pool_alloc<double>();
  std::vector<double*> ptr_vec;
  double* pre_ptr = pool_allocator.allocate(1);
  ptr_vec.push_back(pre_ptr);
  for(int i = 0; i < 10; ++i){
    double* ptr = pool_allocator.allocate(1);
    LOG(INFO) << "ptr memory address is " << ptr;
    LOG(INFO) << "address distance is " << (ptr - pre_ptr) * sizeof(double) << " [byte]";
    pre_ptr = ptr;
    ptr_vec.push_back(pre_ptr);
  } 
  for(auto ptr : ptr_vec){
    pool_allocator.deallocate(ptr, 0);
  }
}

// 我们以Screen对象的分配来看如何一步一步的引入内存池的构建
class Screen{
  public:
    Screen(int x) : i_(x){}
    int get() const{return i_;}
    static void* operator new(size_t);
    static void operator delete(void*, size_t);
  private:
    int i_; // 一个私有变量
  private:
    Screen* next_; // 维护内存链表
    static Screen* free_store_;
    static const int screen_chunk_;
};

Screen* Screen::free_store_ = nullptr;
const int Screen::screen_chunk_ = 4;

void* Screen::operator new(size_t size){
  Screen* p = nullptr;
  if(!free_store_){
    // 这里一次性分配4 x sizeof(Screen)的地址，然后将他们用一个链表串联起来
    // 然后调用new expression的时候就分配出一块内存出去，然后free_store_就向
    // 后面移动一位, 如果内存不够了的话则重新分配4 x sizeof(Screen)大小，并且
    // 和之前的内存链表链接起来
    size_t chunk =  screen_chunk_ * size; // size is sizeof(Screen) 
    free_store_ = p = reinterpret_cast<Screen*>(new char[chunk]);
    for(; p != &free_store_[screen_chunk_ - 1]; ++p){
      p->next_ = p + 1;
    }
    p->next_ = nullptr;
  }
  p = free_store_;
  free_store_ = free_store_->next_;
  return p;
}
void Screen::operator delete(void* ptr, size_t){
  // 在调用delete的时候并没有实际将内存返回给glibc库，而是将free_store_的指针
  // 向前移动了一下，如果后面还有内存要分配就直接给出去就好
  static_cast<Screen*>(ptr)->next_ = free_store_;
  free_store_ = static_cast<Screen*>(ptr);
}

TEST_F(TestStdAllocator, ScreenAllocator){
  size_t size = sizeof(Screen); // size = 16
  std::vector<Screen*> ptr_vec;
  Screen* pre_ptr = new Screen(0);
  ptr_vec.push_back(pre_ptr);
  for(int i = 0; i < 3; ++i){
    Screen* ptr = new Screen(i + 1);
    EXPECT_EQ((ptr - pre_ptr) * size, size); // 我们可以看到前4个对象地址之间的距离都是16
    LOG(INFO) << "pointer address distance is " << (ptr - pre_ptr) * size;
    pre_ptr = ptr;
    ptr_vec.push_back(pre_ptr);
  }
  // 分配第5个对象的时候发现free_store_已经变成null了，说明没有内存可以用了，因此会在此
  // 向系统申请4 x sizeof(Screen)大小的内存，然后串成链表放到内存池中
  Screen* ptr_5 = new Screen(5);
  EXPECT_GT((ptr_5 - pre_ptr) * size, size); 
  // 开始delete前面四个对象
  for(int i = 0; i < 4; ++i){
    delete ptr_vec[i];
  }
  // 再此重新分配4个对象
  std::vector<Screen*> new_ptr_vec;
  for(int i = 0; i < 4; ++i){
    Screen* ptr = new Screen(i);
    new_ptr_vec.push_back(ptr);
  }
  // new_ptr_vec 是不是存的内存和ptr_vec是一样的呢？
  for(int i = 0; i < 4; ++i){
    EXPECT_EQ(ptr_vec[i], new_ptr_vec[3 - i]); // 其实值是反着过来的
  }
}

// 总的来说我们实现了第一版本的Screen分配器，但是也付出了一定的代价，因为额外需要一个
// next_指针来维护内存链表,其实next_指针只有对维护空闲的内存块有用，如果这个内存被分配
// 出去了next_指针就没啥用处了。对于空闲的内存块，分配给Screen私有变量i_的内存其实没啥用
// 因为里面无所谓要放什么值，因此我们可以看到next_指针和私有变量的使用时间正好可以分开来
// 那么我们就借用C语言的union联合体进行下一步的改造

class ScreenV2{
  public:
    ScreenV2(int x) : i_(x){}
    int get() const{return i_;}
    static void* operator new(size_t);
    static void operator delete(void*, size_t);
  private:
    union{
      int i_; // 一个私有变量
      ScreenV2* next_; // 维护内存链表
    };
  private:
    static ScreenV2* free_store_;
    static const int screen_chunk_;
};

ScreenV2* ScreenV2::free_store_ = nullptr;
const int ScreenV2::screen_chunk_ = 4;

void* ScreenV2::operator new(size_t size){
  ScreenV2* p = nullptr;
  if(!free_store_){
    size_t chunk =  screen_chunk_ * size; // size is sizeof(Screen) 
    free_store_ = p = reinterpret_cast<ScreenV2*>(new char[chunk]);
    for(; p != &free_store_[screen_chunk_ - 1]; ++p){
      p->next_ = p + 1;
    }
    p->next_ = nullptr;
  }
  p = free_store_;
  free_store_ = free_store_->next_;
  return p;
}
void ScreenV2::operator delete(void* ptr, size_t){
  static_cast<ScreenV2*>(ptr)->next_ = free_store_;
  free_store_ = static_cast<ScreenV2*>(ptr);
}

TEST_F(TestStdAllocator, ScreenV2Allocator){
  // 我们可以看到现在这个ScreenV2的内存消耗变成了8个字节，比Screen小了8个字节
  std::vector<ScreenV2*> ptr_vec;
  EXPECT_EQ(sizeof(ScreenV2), 8);
  LOG(INFO) << "sizeof(ScreenV2) is " << sizeof(ScreenV2); 
  ScreenV2* pre_ptr = new ScreenV2(0);
  ptr_vec.push_back(pre_ptr);

  for(int i = 0; i < 3; ++i){
    ScreenV2* ptr = new ScreenV2(i + 1);
    LOG(INFO) << "pointer address distance is " << (ptr - pre_ptr) * sizeof(ScreenV2);
    pre_ptr = ptr;
    ptr_vec.push_back(pre_ptr);
  }
  for(int i = 0; i < 4; ++i){
    // 当内存已经分配出去使用之后union联合体的内存就是给私有变量i来使用了
    EXPECT_EQ(ptr_vec[i]->get(), i);
  }
}

// 看到第二版本的内存池的设计我们看到已经节省了不必要的内存开销了，但是有个问题就是我们
// 不可能针对每一种类都去写一下他的内存池相关的代码吧，我们其实可以将其共有的部分抽取出来
// 然后写成一个allocator

class MyAllocator{
  public:
    void* allocate(size_t);
    void deallocate(void*);
  private:
    struct obj{
      struct obj* next_;
    };
    obj* free_store_ = nullptr;
    const int CHUNK = 4;
};

void* MyAllocator::allocate(size_t size){
  obj* p = nullptr;
  if(!free_store_){
    // linked list为空，需要申请一大块内存
    size_t chunk = CHUNK * size;
    free_store_ = p = reinterpret_cast<obj*>(new char[chunk]);
    // 将分配到的内存切成一小块一小块串联起来
    for(int i = 0; i < CHUNK - 1; ++i){
      p->next_ = reinterpret_cast<obj*>(reinterpret_cast<char*>(p) + size);
      p = p->next_;
    }
    p->next_ = nullptr;
  }
  p = free_store_;
  free_store_ = free_store_->next_;
  return p;
}

void MyAllocator::deallocate(void* p){
  reinterpret_cast<obj*>(p)->next_ = free_store_;
  free_store_ = reinterpret_cast<obj*>(p);
}

class ScreenWithAllocator{
  public:
    ScreenWithAllocator(){}
    static void* operator new(size_t size){
      return my_alloc_.allocate(size);
    }
    static void operator delete(void* ptr){
      return my_alloc_.deallocate(ptr);
    }
  private:
    int data_; // 如果将这个换成8字节的对象的话下面代码就会通过
    static MyAllocator my_alloc_;
};
MyAllocator ScreenWithAllocator::my_alloc_;

TEST_F(TestStdAllocator, test_screen_with_usr_defined_allocator){
  EXPECT_EQ(sizeof(ScreenWithAllocator), 4);
  // 我们可以看到将内存池分配交给专门的allocator之后，Screen的内存大小为4bytes
  LOG(INFO) << "sizeof(ScreenWithAllocator) is " << sizeof(ScreenWithAllocator);
  EXPECT_EQ(sizeof(MyAllocator), 16);
  std::vector<ScreenWithAllocator*> ptr_vec;
  // 注意哈，如果我们像下面这么用会发生严重的段错误,究其原因是调用operator new(size_t size)
  // 传入的大小是4，而allocator本身的指针大小就是8了，因此会出现内存bug。其实MyAllocator
  // 接受的最小内存大小应该是8个字节的，但是这里不进行改进了，后面我们可以看到标准库中的内存
  // 池设计
  // for(int i = 0; i < 4; ++i){
  //   ScreenWithAllocator* ptr = new ScreenWithAllocator();
  //   ptr_vec.push_back(ptr);
  // }
}

// 我们简单看一下MyAllocator的缺点，发现不同的类都要维护一个static alloc，然后这个alloc
// 只能管理固定大小的内存池，这并不通用，为了减少每个类都维护一个static alloc，我们可以设计
// 一个较为通用类型的内存池，即它里面维护内存池不是通过一个链表来维护，而是通过一堆链表来进行
// 维护，比如有16个链表来进行维护，第一个链表来分配内存大小为8字节的请求，第二个链表则是16字节
// 等等以此类推，这就是stl中的pool allocator的想法，我们将在后文有对应的介绍



// STL库中的allocator：
// 1、ext/new_allocator.h 这个是STL的标准分配器
// 2、ext/pool_allocator.h 曾经的STL标准分配器，但是现在不是作为默认的标准分配器了
// 3、ext/malloc_allocator.h 底层调用malloc和free函数管理内存，和new_allocator.h差不多
// 4、ext/mt_allocator.h 多线程内存池分配器
// 5、ext/debug_allocator.h
// 5、ext/array_allocator.h
// 6、ext/throw_allocator.h
// 7、ext/bitmap_allocator.h
// 8、ext/extptr_allocator.h

// 下面我们可以翻开STL的源代码看看里面都有些什么东西
// 首先我们可以开始看STL库中标准的分配器 std::allocator<T> // bits/allocator.h
// std::allocator<T> 继承自__allocator_base<T>  //in bits/c++allocator.h header file
// __allocator_base<T>是__gnu_cxx::new_allocator<_Tp>一个别名  // in ext/new_allocator.h header file
// new_allocator是c++标准库中默认的分配器，简单看一下他的代码，它就是对::operator new和
// ::operator delete 包装了一下
// 
// 内存分配函数，调用operator new来进行内存分配
// pointer allocate(size_type __n, const void* = 0){
//   if(__n > this->max_size()){
//     std::__throw_bad__alloc();
//   }
//   return static_cast<_Tp*>(::operator new(__n * sizeof(_Tp)));
// }
// 内存回收函数，调用operator delete来进行内存回收
// void deallocate(pinter __P, size_type){
//   ::operator delete(__p); 类似free
// }
//
// 构造函数，调用placement new来对内存区块进行构造
// void construct(pointer& __p, const _Tp& __val){
//   ::new((void*)__p)_Tp(__val);
// }
//
// 析构函数，直接调用对象的析构函数来进行析构操作
// void destroy(pointer __p){
//   __p->~Tp();
// }

class Widget1{
  public:
    Widget1(int data) : 
      data_(data){}
    int get_data() const{return data_;}
  private:
    int data_;
};

TEST_F(TestStdAllocator, new_allocator_allocate_and_deallocate){
  __gnu_cxx::new_allocator<Widget1> alloc;
  std::vector<Widget1*> ptr_vec;
  for(int i = 0; i < 10; ++i){
    ptr_vec.push_back(alloc.allocate(1));
  }
  for(int i = 0; i < 10; ++i){
    alloc.construct(ptr_vec[i], i);
  }
  for(int i = 0; i < 10; ++i){
    EXPECT_EQ(ptr_vec[i]->get_data(), i);
  }
  for(int i = 0; i < 10; ++i){
    alloc.destroy(ptr_vec[i]);
  }
  for(int i = 0; i < 10; ++i){
    alloc.deallocate(ptr_vec[i], 0);
  } 

  // 前面我们是一个一个分配对象的，那我们也可以一次性分配10个内存
  Widget1* ptr = alloc.allocate(10);
  for(int i = 0; i < 10; ++i){
    alloc.construct(&ptr[i], i);
    EXPECT_EQ(ptr[i].get_data(), i);
  }
  // 记得调用10次析构函数之后在进行释放内存
  for(int i = 0; i < 10; ++i){
    alloc.destroy(&ptr[i]);
  }
  alloc.deallocate(ptr, 0);
}

// 在src/pool_allocator.cc中我们添加了打印代码
TEST_F(TestStdAllocator, test_usr_defined_pool_allocator){
  __gnu_cxx::__pool_alloc<double> pool_alloc;
  double* ptr = pool_alloc.allocate(10);
  pool_alloc.deallocate(ptr, 0);
}

TEST_F(TestStdAllocator, test_pool_allocator){
  if (getenv("GLIBCXX_FORCE_NEW")){
    return;
  }
  {
    // 如果是分配char大小的对象的话，其实给回来的内存块是8个字节
    __gnu_cxx::__pool_alloc<char> pool_alloc;
    char* pre_ptr = pool_alloc.allocate(1);
    for(int i = 0; i < 19; ++i){
      char* ptr = pool_alloc.allocate(1);
      EXPECT_EQ(ptr - pre_ptr, 8);
      LOG(INFO) << "char pointer address distance is " << (ptr - pre_ptr) * sizeof(char);
      pre_ptr = ptr;
    }
  }
  {
    // 如果是分配的int大小的对象的话，pool内存池给回来的内存块也是8个字节
    __gnu_cxx::__pool_alloc<int> pool_alloc;
    int* pre_ptr = pool_alloc.allocate(1);
    for(int i = 0; i < 19; ++i){
      int* ptr = pool_alloc.allocate(1);
      EXPECT_EQ((ptr - pre_ptr) * sizeof(int), 8);
      LOG(INFO) << "int pointer address distance is " << (ptr - pre_ptr) * sizeof(int);
      pre_ptr = ptr;
    }
  }
  {
    // 分配对象大小为128个字节
    class Widget{
      public:
        Widget();
      private:
        // std::string str[4];
        char data[128];
    };
    __gnu_cxx::__pool_alloc<Widget> pool_alloc;
    EXPECT_EQ(sizeof(Widget), 128);
    Widget* pre_ptr = pool_alloc.allocate(1);
    for(int i = 0; i < 10; ++i){
      Widget* ptr = pool_alloc.allocate(1);
      EXPECT_EQ((ptr - pre_ptr) * sizeof(Widget), 128);
      LOG(INFO) << "Widget pointer address distance is " << (ptr - pre_ptr) * sizeof(Widget);
      pre_ptr = ptr;
    }
  }
  {
    // 分配大小为136个字节，这么大的对象已经不从内存池中分配内存了
    class Widget1{
      public:
        Widget1(){}
      private:
        // std::string str[4];
        // long data;
        char data[136];
    };
    __gnu_cxx::__pool_alloc<Widget1> pool_alloc;
    EXPECT_EQ(sizeof(Widget1), 136);
    Widget1* pre_ptr = pool_alloc.allocate(1);
    for(int i = 0; i < 8; ++i){
      Widget1* ptr = pool_alloc.allocate(1);
      EXPECT_GT((ptr - pre_ptr) * sizeof(Widget1), 136);
      LOG(INFO) << "Widget1 pointer address distance is " << 
        (ptr - pre_ptr) * sizeof(Widget1);
    }
  }
}


TEST_F(TestStdAllocator, pool_allocator_use_new_operator){
  // must set GLIBCXX_FORCE_NEW when run test
  if (getenv("GLIBCXX_FORCE_NEW")){
    __gnu_cxx::__pool_alloc<double> pool_alloc;
    double* pre_ptr = pool_alloc.allocate(1);
    for(int i = 0; i < 10; ++i){
      double* ptr = pool_alloc.allocate(1);
      EXPECT_GT((ptr - pre_ptr) * sizeof(double), sizeof(double));
      LOG(INFO) << "pointer address distance is " << (ptr - pre_ptr) * sizeof(double);
      pre_ptr = ptr;
    }
  } 
}


class TestMallocAllocator : public ::testing::Test{};
TEST_F(TestMallocAllocator, test_equal){
  __gnu_cxx::malloc_allocator<int> int_malloc;
  LOG(INFO) << "memory max size is " << int_malloc.max_size();
  int data = 3;
  EXPECT_EQ(&data, int_malloc.address(data));
  int* ptr = int_malloc.allocate(sizeof(int));
  EXPECT_EQ(ptr, int_malloc.address(*ptr));
  int_malloc.construct(ptr, 10);
  EXPECT_EQ(*ptr, 10);
  typedef typename __gnu_cxx::malloc_allocator<int>::size_type size_type;
  int_malloc.deallocate(ptr, size_type(1));
}

TEST_F(TestMallocAllocator, test_construct_destroy){
  class Person{
    public:
      Person(std::string& name, int age) : 
        name_(name), age_(age){}
      ~Person(){
        LOG(INFO) << "dector person";
      }
    private:
      std::string name_;
      int age_;
  };

  __gnu_cxx::malloc_allocator<Person> person_malloc;
  Person* ptr = person_malloc.allocate(sizeof(Person));
  std::string name = "john";
  Person man(name, 10);
  person_malloc.construct(ptr, man);
  person_malloc.destroy(ptr);
}

