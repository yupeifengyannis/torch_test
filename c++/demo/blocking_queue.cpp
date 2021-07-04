/*************************************************************************
  > File Name: test/demo/blocking_queue.cpp
  > Author: yupeifeng
  > Mail: yupeifengyannis@foxmail.com
  > Created Time: 2020年11月24日 星期二 23时04分30秒
 ************************************************************************/

#include <queue>
#include <thread>
#include <string>
#include <condition_variable>
#include <vector>
#include <mutex>
#include <functional>
using namespace std;

template<typename T>
class BlockingQueue {
 public:
  BlockingQueue();
  ~BlockingQueue();

  void push(const T& t);
  // This logs a message if the threads needs to be blocked
      // useful for detecting e.g. when data feeding is too slow
  T pop(const char* log_on_wait);
  T pop();

  bool try_peek(T* t);
  bool try_pop(T* t);

  // Return element without removing it
  T peek();

  size_t size() const;
  bool nonblocking_size(size_t* size) const;

 protected:
  std::queue<T> queue_;
  mutable std::mutex mutex_;
  std::condition_variable condition_;
};

template<typename T>
BlockingQueue<T>::BlockingQueue() {}

template<typename T>
BlockingQueue<T>::~BlockingQueue() {}

template<typename T>
void BlockingQueue<T>::push(const T& t) {
  std::lock_guard<mutex> lock(mutex_);
  queue_.push(t);
  condition_.notify_one();
}

template<typename T>
bool BlockingQueue<T>::try_pop(T* t) {
    boost::mutex::scoped_lock lock(mutex_);
    if (queue_.empty()) {
          return false;
            
    }
      *t = queue_.front();
        queue_.pop();
          return true;
}

template<typename T>
T BlockingQueue<T>::pop(const char* log_on_wait) {
    boost::mutex::scoped_lock lock(mutex_);
    while (queue_.empty()) {
          LOG_EVERY_N(INFO, 10000) << log_on_wait;
              condition_.wait(lock);
                
    }
      T t = queue_.front();
        queue_.pop();
          return t;

}

template<typename T>
T BlockingQueue<T>::pop() {
    boost::mutex::scoped_lock lock(mutex_);
    while (queue_.empty()) {
          condition_.wait(lock);
            
    }
      T t(queue_.front());
        queue_.pop();
          return t;

}

template<typename T>
bool BlockingQueue<T>::try_peek(T* t) {
    boost::mutex::scoped_lock lock(mutex_);
    if (queue_.empty()) {
          return false;
            
    }
      *t = queue_.front();
        return true;

}

template<typename T>
T BlockingQueue<T>::peek() {
    boost::mutex::scoped_lock lock(mutex_);
    while (queue_.empty()) {
          condition_.wait(lock);
            
    }
      return queue_.front();

}

template<typename T>
size_t BlockingQueue<T>::size() const {
    boost::mutex::scoped_lock lock(mutex_);
      return queue_.size();
}

template<typename T>
bool BlockingQueue<T>::nonblocking_size(size_t* size) const {
    boost::mutex::scoped_lock lock(mutex_, boost::try_to_lock);
    if (lock.owns_lock()) {
          *size = queue_.size();
              return true;
    }
      return false;
}
