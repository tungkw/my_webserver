#include "timer.h"
#include <iostream>


TimerHeap::TimerHeap(){
    timer_heap = new std::vector<TimerNode>;
    time_log = new std::map<int, time_t>;
}

TimerHeap::~TimerHeap(){
    delete timer_heap;
    delete time_log;
}

void TimerHeap::remove(int fd){
    // std::lock_guard<std::mutex> locker(mtx);
    if(time_log->find(fd) != time_log->end()){
        time_log->erase(time_log->find(fd));
    }
}

void TimerHeap::check(){
    std::lock_guard<std::mutex> locker(mtx);
    if(!timer_heap->empty()){
        time_t t = time(nullptr);
        while(!timer_heap->empty()){
            auto top = timer_heap->begin();
            if(time_log->find(top->fd) == time_log->end()){
                std::pop_heap(timer_heap->begin(), timer_heap->end());
                timer_heap->pop_back();
            }
            else if(time_log->at(top->fd) > top->time){
                top->time = time_log->at(top->fd);
                std::make_heap(timer_heap->begin(), timer_heap->end());
            }
            else if(t - top->time > top->time_out){
                std::pop_heap(timer_heap->begin(), timer_heap->end());
                LOG_INFO("%d time out", timer_heap->back().fd);
                timer_heap->back().time_out_cb();
                timer_heap->pop_back();
            }
            else{
                break;
            }
        }
    }
}

void TimerHeap::update(int fd){
    // std::lock_guard<std::mutex> locker(mtx);
    (*time_log)[fd] = time(nullptr);
}

time_t TimerHeap::get_waittime(){
    std::lock_guard<std::mutex> locker(mtx);
    time_t waittime = -1;
    if(!timer_heap->empty()){
        waittime = timer_heap->front().time + timer_heap->front().time_out - time(nullptr);
    }
    return waittime >= 0? waittime : 0;
}