#ifndef __M_SERVER_H
#define __M_SERVER_H
#include <map>
#include "epoll.h"
#include "thread_pool.h"
#include "client.h"
#include "log.h"

class Server{
public:
    Server(short port=10000, int max_nfd=1024);
    ~Server();
    void start();
private:
    void init_socket();
    void add_client();
    void del_client(Client *client);
    void deal_read(Client *client);
    void deal_write(Client *client);

    short port;
    int fd_listen;
    Epoll *epoll_handler;
    ThreadPool *thread_pool;
    std::map<int, Client> clients;
};

#endif