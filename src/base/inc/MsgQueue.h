#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>


namespace model {

template<typename T>
class MsgQueue {
    public:
        MsgQueue() {}
        ~MsgQueue() {};

    void Push(T value) {
        mutex_.lock();
        queue_.push(std::move(value));
        mutex_.unlock();
        cond_queued_.notify_all();
    }

    T& Front() {
        std::unique_lock<std::mutex> lk(mutex_);
        return queue_.front();
    }

    T& WaitAndFront() {
        std::unique_lock<std::mutex> lk(mutex_);
        cond_queued_.wait(lk, [this]{return !queue_.empty();});
        return queue_.front();
    }

    T const& Front() const {
        std::unique_lock<std::mutex> lk(mutex_);
        return queue_.front();
    }

    T WaitAndPop() {
        std::unique_lock<std::mutex> lk(mutex_);
        cond_queued_.wait(lk, [this]{return !queue_.empty();});
        T popped_value = std::move(queue_.front());
        queue_.pop();
        return popped_value;
    }

    T Pop() {
        mutex_.lock();
        T popped_value = std::move(queue_.front());
        queue_.pop();
        mutex_.unlock();
        cond_done_.notify_all();
        return popped_value;
    }

    bool Empty() {
        std::unique_lock<std::mutex> lk(mutex_);
        return queue_.empty();
    }

    uint32_t Size() {
        std::unique_lock<std::mutex> lk(mutex_);
        return queue_.size();
    }

    void WaitEmpty() {
        std::unique_lock<std::mutex> lk(mutex_);
        cond_done_.wait(lk, [this]{return queue_.empty();});
    }

    void SetName(std::string name) {
        name_ = name;
    }

    std::string& Name() {
        return name_;
    }

private:
	std::queue<T>               queue_;
	std::mutex                  mutex_;
	std::condition_variable_any cond_queued_;
	std::condition_variable_any cond_done_;
    std::string                 name_;
};
}
