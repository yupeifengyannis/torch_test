#include <iostream>

class Screen{
public:
    Screen(int x) : i_(x){}
    int get() const{return i_;}
    static void* operator new(size_t);
    static void operator delete(void*, size_t);
private:
    int i_;

private:
    Screen* next_;
    static Screen* free_store_;
    static const int screen_chunk_;
};

Screen* Screen::free_store_ = nullptr;
const int Screen::screen_chunk_ = 24;

void* Screen::operator new(size_t size){
    Screen* p = nullptr;
    if(!free_store_){
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
