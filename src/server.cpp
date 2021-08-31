#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include "server.h"

Server::Server(
    short port,
    int max_nfd
){
    this->port = port;
    thread_pool = new ThreadPool();
    epoll_handler = new Epoll(max_nfd);
    init_socket();
}

Server::~Server(){
    delete epoll_handler;
}

void Server::start(){
    std::cout << "started" << std::endl;
    
    while(true){
        int num_events = epoll_handler->wait(-1);
        if(num_events == -1){
            perror("wait");
            exit(-1);
        }
        
        for(int i=0; i<num_events; i++){
            int fd = epoll_handler->get_fd(i);
            uint32_t events = epoll_handler->get_events(i);

            if(fd == fd_listen){
                add_client();
            }
            else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
                //del_client();
            }
            else if(events & EPOLLIN){
                //deal_read(fd);
                thread_pool->add_task(std::bind(&Server::deal_read, this, fd));
            }
            else if(events & EPOLLOUT){
                //deal_write();
            }
            else{
                
            }
        }
    }
}

void Server::init_socket(){
    int ret;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    fd_listen = socket(PF_INET, SOCK_STREAM, 0);

    int optval = 1;
    setsockopt(fd_listen, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    bind(fd_listen, (sockaddr*)&addr, sizeof(addr));

    ret = epoll_handler->add_fd(fd_listen, EPOLLIN);
    if(ret == -1){
        perror("add listen");
        exit(-1);
    }

    ret = listen(fd_listen, 8);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }
}

void Server::add_client(){
    std::cout << "new client" << std::endl;
    sockaddr_in addr;
    socklen_t len;
    while(true){
        int fd = accept(fd_listen, (sockaddr*)&addr, &len);
        if(fd <= 0){return;}
        epoll_handler->add_fd(fd, EPOLLIN);
    }
}

void Server::del_client(int fd){
    std::cout << "client close" << std::endl;
    epoll_handler->del_fd(fd);
    close(fd);
}

void Server::deal_read(int fd){
    char buff[10];
    while(true){
        int len = read(fd, buff, 10);
        if(len > 0){
            std::cout << buff << std::endl;
        }
        else if(len == 0){
            del_client(fd);
            break;
        }
        else if(len == -1 && errno != EAGAIN){
            perror("read");
            exit(-1);
        }
        else{
            break;
        }
    }
}

void Server::deal_write(int fd){

}