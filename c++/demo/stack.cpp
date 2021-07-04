#include <list>
#include <stack>
#include <deque>
#include <vector>
#include <list>
#include <iostream>
using namespace std;
template <typename Dtype, typename Container>
void test_stack(){
    std::stack<Dtype, Container> s;
    for(int i = 0; i < 10; ++i){
        s.push(i);
    }
    for(int i = 0; i < 10; ++i){
        std::cout << s.top() << " ";
        s.pop();
    }
    std::cout << std::endl;
}
int main(){
    test_stack<int, deque<int>>();
    test_stack<int, vector<int>>();
    test_stack<int, list<int>>();
}