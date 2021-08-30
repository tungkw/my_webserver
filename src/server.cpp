#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include "server.h"

Server::Server(
    short port,
    int max_nfd
){
    this->port = port;
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
        std::cout << num_events << std::endl;
        for(int i=0; i<num_events; i++){
            int fd = epoll_handler->get_fd(i);
            uint32_t events = epoll_handler->get_events(i);

            std::cout << fd << std::endl;
            if(fd == fd_listen){
                std::cout << "requesting" << std::endl;
                
            }
            else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){

            }
            else if(events & EPOLLIN){

            }
            else if(events & EPOLLOUT){

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
    ret = listen(fd_listen, 8);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }

    ret = epoll_handler->add_fd(fd_listen, EPOLLIN);
    if(ret == -1){
        perror("add listen");
        exit(-1);
    }

    //ret = fcntl(fd_listen, F_SETFL, fcntl(fd_listen, F_GETFD, 0) | O_NONBLOCK);
    //if(ret == -1){
    //    perror("set nonblock");
    //    exit(-1);
    //}
}