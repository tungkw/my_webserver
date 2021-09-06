#include "epoll.h"

Epoll::Epoll(int max_events){
    fd_epoll = epoll_create(512);
    if(fd_epoll == -1){
        perror("create epoll");
        exit(-1);
    }
    epoll_ret = std::vector<epoll_event>(max_events);
}

Epoll::~Epoll(){
    close(fd_epoll);
}

bool Epoll::add_fd(int fd, uint32_t events){
    set_block(fd, false);
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events | EPOLLET | EPOLLONESHOT;
    return epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd, &ev);
}

bool Epoll::mod_fd(int fd, uint32_t events){
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events | EPOLLET | EPOLLONESHOT;
    return epoll_ctl(fd_epoll, EPOLL_CTL_MOD, fd, &ev);
}

bool Epoll::del_fd(int fd){
    epoll_event ev = {0};
    ev.data.fd = fd;
    return epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd, &ev);
}

int Epoll::wait(int time_out){
    return epoll_wait(fd_epoll, &epoll_ret[0], epoll_ret.size(), time_out);
}

int Epoll::get_fd(int i){
    return epoll_ret[i].data.fd;
}

uint32_t Epoll::get_events(int i){
    return epoll_ret[i].events;
}

void Epoll::set_block(int fd, bool block){
    int ret;
    if(block){
        ret = fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) & ~(O_NONBLOCK));
    }
    else{
        ret = fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
    }
    if(ret == -1){
        perror("set block");
        std::cout << fd << std::endl;
        exit(-1);
    }
}
