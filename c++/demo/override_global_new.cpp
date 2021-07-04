#include <iostream>
void *operator new(size_t size)
{
    std::cout << "::operator new is overrided" << std::endl;
    return malloc(size);
}
void operator delete(void *ptr)
{
    std::cout << "::operator delete is overrided" << std::endl;
    free(ptr);
}
void *operator new[](size_t size)
{
    std::cout << "::operator new[] is overrided" << std::endl;
    return malloc(size);
}
void operator delete[](void *ptr)
{
    std::cout << "::operator delete[] is overrided" << std::endl;
    free(ptr);
}
int main(){
    int* a = new int(1);
    delete a;
    int* ptr = new int[3];
    delete[] ptr;
}
