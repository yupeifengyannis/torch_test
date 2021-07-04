#include <iostream>
class Screen{
public:
    Screen(int x) : i_(x){}
    int get() const{return i_;}
    static void* operator new(size_t);
    static void operator delete(void*, size_t);
private:
    union{
        int i_;
        Screen* next_;
    };
private:
    static Screen* free_store_;
    static const int screen_chunk_;
};

Screen* Screen::free_store_ = nullptr;
const int Screen::screen_chunk_ = 24;

void* Screen::operator new(size_t size){
    Screen* p = nullptr;
    if(!free_store_){
        // 如果linked_list是空的，那么申请一大块内存
        size_t chunk =  screen_chunk_ * size; // size is sizeof(Screen) 
        // 将一大块内存分割切片，然后组成一个linked_list
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
    // 将deleted掉的内存插回到free_list的前端，等待下次使用
    static_cast<Screen*>(ptr)->next_ = free_store_;
    free_store_ = static_cast<Screen*>(ptr);
}
int main(){
    std::cout << "sizeof(Screen) is " << sizeof(Screen) << std::endl;
    int N = 100;
    Screen* p[N];
    for(int i = 0; i < N; ++i){
        p[i] = new Screen(i);
    }
    std::cout << "==============use class allocator=============" << std::endl;
    for(int i = 0; i < 10; ++i){
        std::cout << "memory address is " << p[i] << std::endl;
    }
    std::cout << "==============use class allocator=============" << std::endl;
    for(int i = 0; i < N; ++i){
        delete p[i];
    }
    std::cout << "==============use global operator new================" << std::endl;
    for(int i = 0; i < N; ++i){
        p[i] = ::new Screen(i);
    }
    for(int i = 0; i < 10; ++i){
        std::cout << "memory address is " << p[i] << std::endl;
    }
    std::cout << "==============use global operator new================" << std::endl;
    for(int i = 0; i < N; ++i){
        ::delete p[i];
    }

}
