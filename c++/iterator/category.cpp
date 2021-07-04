#include <vector>
#include <deque>
#include <array>
#include <list>
#include <forward_list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <stack>
#include <queue>
#include <cstddef>
#include <iostream>
#include <bits/stl_iterator.h>
using namespace std;

template <typename IteratorCategory>
void print(const string& name, IteratorCategory){

}

template <>
void print(const string& name, std::output_iterator_tag){
    std::cout << "container " << name << "'s iterator category is output_iterator" << std::endl; 
}
template <>
void print(const string& name, std::input_iterator_tag){
    std::cout << "container " << name << "'s iterator category is input_iterator" << std::endl; 
}
template <>
void print(const string& name, std::forward_iterator_tag){
    std::cout << "container " << name << "'s iterator category is forward_iterator" << std::endl; 
}
template <>
void print(const string& name, std::bidirectional_iterator_tag){
    std::cout << "container " << name << "'s iterator category is bidirectional_iterator" << std::endl; 
}
template <>
void print(const string& name, std::random_access_iterator_tag){
    std::cout << "container " << name << "'s iterator category is random_access_iterator" << std::endl; 
}

//template <typename T>
//struct traits{
//    typedef typename T::iterator_category iterator_category;
//    typedef typename T::value_type value_type;
//    typedef typename T::difference_type difference_type;
//    typedef typename T::pointer pointer;
//    typedef typename T::reference reference;
//};
//template <typename T>
//struct traits<T*>{
//    typedef std::random_access_iterator_tag iterator_category;
//    typedef T value_type;
//    typedef ptrdiff_t difference_type;
//    typedef T* pointer;
//    typedef T& reference;
//};
//template <typename T>
//struct traits<const T*>{
//    typedef std::random_access_iterator_tag iterator_category;
//    typedef T value_type;
//    typedef ptrdiff_t difference_type;
//    typedef const T* pointer;
//    typedef T& reference;
//};

void test(){
    print("vector", iterator_traits<vector<int>::iterator>::iterator_category());    
    print("deque", iterator_traits<deque<int>::iterator>::iterator_category());
    print("array", iterator_traits<array<int,1>::iterator>::iterator_category());
    print("list", iterator_traits<list<int>::iterator>::iterator_category());
    print("forward_list", iterator_traits<forward_list<int>::iterator>::iterator_category());
    print("set", iterator_traits<set<int>::iterator>::iterator_category());
    print("map", iterator_traits<map<int, int>>::iterator_category());
    print("unordered_set", iterator_traits<unordered_set<int>>::iterator_category());
    print("unordered_map", iterator_traits<unordered_map<int, int>>::iterator_category());
    print("string", iterator_traits<string::iterator>::iterator_category());
    print("int*", iterator_traits<int*>::iterator_category());
    print("const double*", iterator_traits<const double*>::iterator_category());

}

int main(){
    test();
}

