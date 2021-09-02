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
                thread_pool->add_task(std::bind(&Server::deal_read, this, clients.at(fd)));
            }
            else if(events & EPOLLOUT){
                thread_pool->add_task(std::bind(&Server::deal_write, this, clients.at(fd)));
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
        if(fd <= 0){break;}
        epoll_handler->add_fd(fd, EPOLLIN);
        clients.insert(std::make_pair(fd, new Client(fd)));
    }
    std::cout << "num of clients " << clients.size() << std::endl;
}

void Server::del_client(int fd){
    std::cout << "client close" << std::endl;
    epoll_handler->del_fd(fd);
    close(fd);
    delete clients.at(fd);
    clients.erase(fd);
}

void Server::deal_read(Client *client){
    if(!client->read_data()) del_client(client->fd);
    if(http_request_sparse(client->read_buffer, client->req)){
        // std::cout << req.act << std::endl;
        // std::cout << req.url << std::endl;
        // std::cout << req.ver << std::endl;
        // for(auto iter = req.feas.begin(); iter != req.feas.end(); iter++){
        //     std::cout << iter->first << " : " << iter->second << std::endl;
        // }
        // std::cout << req.cont << std::endl;
        client->read_buffer.clear();
        process(client->req, client->res);
        client->req = HTTPRequest();
        http_response_build(client->res, client->write_buffer);
        epoll_handler->mod_fd(client->fd, EPOLLOUT);
    }
}

void Server::deal_write(Client *client){
    std::cout << "write" << std::endl;
    std::cout << client->write_buffer << std::endl;
    std::cout << client->write_buffer.length() << std::endl;
    std::cout << write(client->fd, client->write_buffer.c_str(), client->write_buffer.length()) << std::endl;
    epoll_handler->mod_fd(client->fd, EPOLLIN);
}