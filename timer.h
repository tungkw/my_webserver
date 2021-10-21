#ifndef __M_TIMER_H
#define __M_TIMER_H
#include <vector>
#include <sys/time.h>
#include <log.h>
#include <algorithm>
#include <functional>
#include <map>
#include <iostream>
#include <mutex>
#include <thread>

struct TimerNode{
    int fd;
    time_t time;
    time_t time_out;
    std::function<void()> time_out_cb;
    bool operator<(const TimerNode& t){
        return time < t.time;
    }
};

class TimerHeap{

private:
    std::vector<TimerNode> *timer_heap;
    std::map<int, time_t> *time_log;
    std::mutex mtx;

public:
    TimerHeap();
    ~TimerHeap();
    template<class F>
    void add(int fd, int time_out, F&& cb){
        TimerNode tn;
        tn.fd = fd;
        tn.time = time(nullptr);
        tn.time_out = time_out;
        tn.time_out_cb = cb;
        {
            std::lock_guard<std::mutex> locker(mtx);
            timer_heap->push_back(tn);
            std::push_heap(timer_heap->begin(), timer_heap->end());
            (*time_log)[fd] = tn.time;
        }
    }
    void remove(int fd);
    void check();
    void update(int fd);
    time_t get_waittime();
};

#endif