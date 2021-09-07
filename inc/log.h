#ifndef __M_LOG_H
#define __M_LOG_H
#include <deque>
#include <string>
#include <sys/time.h>
#include <stdarg.h>
#include <mutex>
#include <condition_variable>
#include <thread>

#define LOG_BASE(level, format, ...)        {Log::Instance()->write(level, format, ##__VA_ARGS__);}
#define LOG_DEBUG(format, ...)              {LOG_BASE(0, format, ##__VA_ARGS__)}
#define LOG_INFO(format, ...)               {LOG_BASE(1, format, ##__VA_ARGS__)}
#define LOG_WARNING(format, ...)            {LOG_BASE(2, format, ##__VA_ARGS__)}
#define LOG_ERROR(format, ...)              {LOG_BASE(3, format, ##__VA_ARGS__)}

class Log{
public:
    static Log* Instance();
    void init(std::string path="./log", int max_line=5000);
    void write(int level, const char *format, ...);

private:
    Log();
    ~Log();
    void thread_func();

    int max_line;
    int cur_line;
    int cur_day;
    std::string path;
    FILE* fp;

    bool is_closed;
    std::thread *write_thread;
    std::mutex mtx;
    std::condition_variable con;
    std::deque<std::string> log_que;
};

#endif