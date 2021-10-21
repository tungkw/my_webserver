#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include "server.h"

Server::Server(
    short port,
    int max_nfd,
    time_t time_out
){
    Log::Instance()->init("./log");
    this->port = port;
    this->time_out = time_out;
    thread_pool = new ThreadPool();
    epoll_handler = new Epoll(max_nfd);
    init_socket();
}

Server::~Server(){
    delete epoll_handler;
    delete thread_pool;
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
        LOG_ERROR("add listen");
        exit(-1);
    }

    ret = listen(fd_listen, 8);
    if(ret == -1){
        perror("start listen");
        LOG_ERROR("start listen");
        exit(-1);
    }

    LOG_INFO("Server Initialized");
}

void Server::start(){
    LOG_INFO("Server Started");
    time_t waittime = -1;
    while(true){
        // timer->check();
        // waittime = timer->get_waittime() * 1000; // sec * 1000
        int waittime = timer_->GetNextTick();

        int num_events = epoll_handler->wait(waittime);
        if(num_events == -1){
            perror("epoll wait");
            LOG_ERROR("epoll wait");
            exit(-1);
        }
        
        for(int i=0; i<num_events; i++){
            int fd = epoll_handler->get_fd(i);
            uint32_t events = epoll_handler->get_events(i);

            if(fd == fd_listen){
                add_client();
            }
            else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
                del_client(&clients.at(fd));
            }
            else if(events & EPOLLIN){
                thread_pool->add_task(std::bind(&Server::deal_read, this, &clients.at(fd)));
            }
            else if(events & EPOLLOUT){
                thread_pool->add_task(std::bind(&Server::deal_write, this, &clients.at(fd)));
            }
            else{
                
            }
        }
    }
}

void Server::add_client(){
    sockaddr_in addr;
    socklen_t len;
    while(true){
        int fd = accept(fd_listen, (sockaddr*)&addr, &len);
        if(fd <= 0){break;}

        epoll_handler->add_fd(fd, EPOLLIN);
        clients.insert(std::make_pair(fd, Client()));
        clients[fd].init(fd);
        // timer->add(fd, time_out, std::bind(&Server::del_client, this, &clients[fd]));
        timer_->add(fd, time_out*1000, std::bind(&Server::del_client, this, &clients[fd]));
        LOG_INFO("connected %d", fd);
    }
    epoll_handler->mod_fd(fd_listen, EPOLLIN);
}

void Server::del_client(Client *client){
    // timer->remove(client->fd);
    epoll_handler->del_fd(client->fd);
    client->close_fd();
    LOG_INFO("closed %d", client->fd);
}

void Server::deal_read(Client *client){
    // timer->update(client->fd);
    timer_->adjust(client->fd, time_out*1000);
    if(!client->read_data()){
        del_client(client);
    }
    else if(http_request_sparse(client->read_buffer, client->req)){
        client->read_buffer.clear();
        process(client->req, client->res);
        client->req = HTTPRequest();
        http_response_build(client->res, client->write_buffer);
        epoll_handler->mod_fd(client->fd, EPOLLOUT);
    }
    else{
        epoll_handler->mod_fd(client->fd, EPOLLIN);
    }
}

void Server::deal_write(Client *client){
    // timer->update(client->fd);
    timer_->adjust(client->fd, time_out*1000);
    if(!client->write_data()){
        del_client(client);
    }
    else{
        client->write_buffer.clear();
        client->res = HTTPResponse();
        del_client(client);
    }
}