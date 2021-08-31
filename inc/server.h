#ifndef __M_SERVER_H
#define __M_SERVER_H
#include "epoll.h"
#include "thread_pool.h"

class Server{
public:
    Server(short port=10000, int max_nfd=1024);
    ~Server();
    void start();
private:
    void init_socket();
    void add_client();
    void del_client(int fd);
    void deal_read(int fd);
    void deal_write(int fd);

    short port;
    int fd_listen;
    Epoll *epoll_handler; // if not pointer, will construct once here and once in constructor;
    ThreadPool *thread_pool;
};

#endif