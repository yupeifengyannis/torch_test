#include <iostream>
class Foo{
   public:
    Foo(){std::cout << "ctor" << std::endl;}
    virtual ~Foo(){std::cout << "dctor" << std::endl;}
    static void* operator new(size_t size){
        std::cout << "invoke Foo's operator new" << std::endl;
        return malloc(size);
    }
    static void operator delete(void* ptr){
        std::cout << "invoke Foo's operator delete" << std::endl;
        free(ptr);
    }

    static void* operator new[](size_t size){
        std::cout << "invoke Foo's operator new[]" << std::endl;
        return malloc(size);
    }
    // static void operator delete[](void* ptr){
    //    std::cout << "invoke Foo's operator delete[]" << std::endl;
    //    return free(ptr);
    //}

};

int main(){
    Foo* ptr = new Foo();
    delete ptr;
    ptr = new Foo[3];
    delete[] ptr;
    int* p = new int(1);
    delete p;
    p = new int[3];
    delete[] p;
}