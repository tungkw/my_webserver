#ifndef __M_SOCKET_H
#define __M_SOCKET_H
#include <iostream>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include "log.h"


class Epoll{
public:
    Epoll(int max_events=1024);
    ~Epoll();
    bool add_fd(int fd, uint32_t events);
    bool mod_fd(int fd, uint32_t events);
    bool del_fd(int fd);
    int wait(int time_out=-1);
    int get_fd(int i);
    uint32_t get_events(int i);
private:
    void set_block(int fd, bool block);

    int fd_epoll;
    std::vector<struct epoll_event> epoll_ret;
};

#endif