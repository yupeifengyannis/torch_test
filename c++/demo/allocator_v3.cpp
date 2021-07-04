#include <cstddef>
#include <iostream>
#include <cstdlib>
#include <string>
#include <complex>
class allocator{
  private:
    struct obj{
      struct obj* next;
    };
  public:
    void* allocate(size_t);
    void deallocate(void*, size_t);
  private:
    obj* free_store_ = nullptr;
    const int chunk_size_ = 5;
};

void* allocator::allocate(size_t size){
  obj* p = nullptr;
  if(!free_store_){
    // linked_list为空，于是申请一大块内存
    size_t chunk = chunk_size_ * size;
    free_store_ = p = static_cast<obj*>(malloc(chunk));
    // 将分配得到的内存切成一小块一小块，然后用linked_list连接起来
    for(int i = 0; i < chunk_size_ - 1; ++i){
      p->next = reinterpret_cast<obj*>(reinterpret_cast<char*>(p) + size);
      p = p->next; 
    }
    p->next = nullptr; // linked_list最后的指针指向空
  }
  p = free_store_;
  free_store_ = free_store_->next;
  return p;
}

void allocator::deallocate(void* ptr, size_t){
  // 将回收的内存块插入到free_list的前端
  static_cast<obj*>(ptr)->next = free_store_;
  free_store_ = static_cast<obj*>(ptr);
}

class Foo{
  public:
    long data_;
    std::string str_;
    static allocator my_alloc_;
  public:
    Foo(long data) : 
      data_(data){}
    static void* operator new(size_t size){
      return my_alloc_.allocate(size);
    }
    static void operator delete(void* ptr, size_t size){
      return my_alloc_.deallocate(ptr, size);
    }
};

class Goo{
  public:
    std::complex<double> c_;
    std::string str_;
    static allocator my_alloc_;
  public:
    Goo(const std::complex<double>& c) : c_(c){}
    static void* operator new(size_t size){
      return my_alloc_.allocate(size);
    }
    static void operator delete(void* ptr, size_t size){
      return my_alloc_.deallocate(ptr, size);
    }
};
allocator Foo::my_alloc_;
allocator Goo::my_alloc_;

int main(){
  Foo* p[100];
  std::cout << "===========show Foo memory===============";
  std::cout << "sizeof(Foo) is " << sizeof(Foo) << std::endl;
  for(int i = 0;i < 23; ++i){
    p[i] = new Foo(i);
    std::cout << "memory address is " << p[i] << std::endl;
  }
  for(int i = 0; i < 23; ++i){
    delete p[i];
    delete p[i];
  }
  for(int i = 0; i < 23; ++i){
    p[i] = new Foo(i);
  }
  for(int i = 0; i < 23; ++i){
    std::cout << p[i] << "  " <<  p[i]->data_ << std::endl;
  }

  std::cout << "===========show Foo memory===============";

  Goo* p_goo[100];
  std::cout << "===========show Goo memory===============";

  std::cout << "sizeof(Goo) is " << sizeof(Goo) << std::endl;
  for(int i = 0; i < 17; ++i){
    p_goo[i] = new Goo(std::complex<double>(i, i));
    std::cout << "memory address is " << p_goo[i] << std::endl;
  }
  for(int i = 0; i < 17; ++i){
    delete p_goo[i];
  }
  std::cout << "===========show Goo memory===============";
}