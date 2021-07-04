#include <cstddef>
#include <iostream>
#include <vector>
#include <set>
#include <list>

template <typename T>
class MyAlloc{
    public:
        typedef T value_type;
        MyAlloc() noexcept{}
        template <typename U>
        MyAlloc(const MyAlloc<U>&) noexcept{}

        T* allocate(std::size_t num){
            return static_cast<T*>(::operator new(num * sizeof(T)));
        }
        T* deallocate(T* ptr, std::size_t num){
            ::operator delete(ptr);
        }
};
template <typename T1, typename T2>
bool operator== (const MyAlloc<T1>&, const MyAlloc<T2>&){
    return true;
}
template <typename T1, typename T2>
bool operator!=(const MyAlloc<T1>&, const MyAlloc<T2>&){
    return false;
}
int main(){
    std::vector<int, MyAlloc<int>> my_vec;
    std::set<int, std::less<int>, MyAlloc<int>> my_set;
    std::list<int, MyAlloc<int>> my_list;
    for(int i = 0; i < 10; ++i){
        my_vec.push_back(i);
        my_set.insert(i);
        my_list.push_back(i);
    }
    for(auto item : my_vec){
        std::cout << item << " ";
    }
    std::cout << std::endl;
    for(auto item : my_set){
        std::cout << item << " ";
    }
    std::cout << std::endl;

}

