#include <deque>
#include <exception>
#include <iostream>
using namespace std;
template <typename Dtype>
class MyStack{
    protected:
        deque<Dtype> c_;
    public:
    class ReadEmptyStack : public std::exception{
        public:
            virtual const char* what() const throw() override{
                return "read empty stack";
            }
    };
    typename deque<Dtype>::size_type size() const{
        return c_.size();
    }
    bool empty() const{
        return c_.empty();
    }
    void push(const Dtype& data){
        c_.push_back(data);
    }
    Dtype pop(){
        if(c_.empty()){
            throw ReadEmptyStack();
        }
        Dtype elem(c_.back());
        c_.pop_back();
        return elem;
    }
    Dtype& top() {
        if(c_.empty()){
            throw ReadEmptyStack();
        }
        return c_.back();
    }
};

int main(){
    try{
        MyStack<int> s;
        s.push(1);
        s.push(2);
        s.pop();
        s.pop();
        s.pop();
    }
    catch(const exception& e){
        std::cout << e.what() << std::endl;
    }
}