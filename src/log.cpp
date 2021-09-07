#include "log.h"
#include <iostream>

Log::Log(){
    max_line = 5000;
    write_thread = NULL;
    fp = NULL;

}

Log::~Log(){
    if(write_thread){
        is_closed = true;
        write_thread->join();
    }
    if(fp){
        fflush(fp);
        fclose(fp);
    }
}

Log* Log::Instance(){
    static Log log;
    return &log;
}

void Log::init(std::string path_, int max_line_){
    path = path_;
    max_line = max_line_;
    cur_line = 0;

    time_t timer = time(nullptr);
    tm *t = localtime(&timer);
    cur_day = t->tm_mday;

    char buff[255];
    int len = snprintf(buff, 255, "%04d_%02d_%02d_%d", 
        t->tm_year+1900, t->tm_mon+1, t->tm_mday, 0);
    std::string file_name(path);
    file_name += "/" + std::string(&buff[0], (&buff[0])+len) + ".log";
    fp = fopen(file_name.c_str(), "a");

    write_thread = new std::thread(&Log::thread_func, this);
}

void Log::write(int level, const char *format, ...){
    if(!fp) return;

    char buff[255];
    int len = 0;

    time_t timer = time(nullptr);
    tm *t = localtime(&timer);
    len += snprintf((&buff[0])+len, 255-len, "%04d-%02d-%02d %02d:%02d:%02d ", 
        t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    
    switch (level)
    {
    case 0:
        len += snprintf((&buff[0])+len, 255-len, "[DEBUG] : ");
        break;
    case 1:
        len += snprintf((&buff[0])+len, 255-len, "[INFO] : ");
        break;
    case 2:
        len += snprintf((&buff[0])+len, 255-len, "[WARNING] : ");
        break;
    case 3:
        len += snprintf((&buff[0])+len, 255-len, "[ERROR] : ");
        break;
    default:
        len += snprintf((&buff[0])+len, 255-len, "[?????] : ");
        break;
    }

    va_list vlist;
    va_start(vlist, format);
    len += vsnprintf((&buff[0])+len, 255-len, format, vlist);
    va_end(vlist);

    len += snprintf((&buff[0])+len, 255-len, "\n");
    
    {
        std::lock_guard<std::mutex> locker(mtx);
        log_que.push_back(std::string(&buff[0], (&buff[0])+len));
    }
    con.notify_one();
}

void Log::thread_func(){
    Log* log = Log::Instance();
    std::unique_lock<std::mutex> locker(log->mtx);
    while(true){
        if(log->log_que.size()){
            std::string msg = log->log_que.front();
            log->log_que.pop_front();
            locker.unlock();
            fputs(msg.c_str(), fp);
            fflush(fp);
            cur_line++;
            time_t timer = time(nullptr);
            tm *t = localtime(&timer);
            if(cur_line%max_line==0 || t->tm_mday!=cur_day){
                fflush(fp);
                fclose(fp);
                char name_[255];
                int name_l = snprintf(name_, 255, "%04d_%02d_%02d_%d", 
                    t->tm_year+1900, t->tm_mon+1, t->tm_mday, cur_line/max_line);
                std::string file_name(path);
                file_name += "/" + std::string(&name_[0], (&name_[0])+name_l) + ".log";
                fp = fopen(file_name.c_str(), "a");
                // cur_line = 0;
                cur_day = t->tm_mday;
            }
            locker.lock();
        }
        else if(log->is_closed) break;
        else{
            log->con.wait(locker);
        }
    }
}
