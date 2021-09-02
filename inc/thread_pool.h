#ifndef __M_THREAD_H
#define __M_THREAD_H
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>


class ThreadPool{
public:
    ThreadPool(int num_thread=8){
        pool_ = new Pool();
        for(int i = 0 ; i < num_thread; i++){
            std::thread(&ThreadPool::thread_func, this, pool_).detach();
        }
    }

    ~ThreadPool(){
        {
            std::lock_guard<std::mutex> lock(pool_->mtx);
            pool_->is_closed = true;
        }
        pool_->cond.notify_all();
    }

    template<class F>
    void add_task(F&& task){
        {
            // lock when constructed, and unlock when leaf this space
            std::lock_guard<std::mutex> lock(pool_->mtx);
            pool_->tasks.emplace(std::forward<F>(task));
        }
        pool_->cond.notify_one();
    }
    
private:
    struct Pool{
        bool is_closed;
        std::mutex mtx;
        std::condition_variable cond;
        std::queue<std::function<void()>> tasks;
    }* pool_;

    void thread_func(Pool *pool){
        std::unique_lock<std::mutex> lock(pool->mtx);
        while(true){
            if(!pool->tasks.empty()){
                auto task = std::move(pool->tasks.front());
                pool->tasks.pop();
                lock.unlock();
                task();
                lock.lock();
            }
            else if(pool->is_closed) break;
            else pool->cond.wait(lock);
        }
    }
};

#endif