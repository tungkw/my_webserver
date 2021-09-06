#ifndef __M_CLIENT_H
#define __M_CLIENT_H
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include "http.h"
#include "process.h"

class Client{
public:
Client():fd(-1), is_closed(true){}
~Client(){close_fd();}
bool read_data();
bool write_data();
void init(int fd_);
void close_fd();

bool is_closed;
int fd;
std::string read_buffer;
std::string write_buffer;
HTTPRequest req;
HTTPResponse res;
};

#endif
