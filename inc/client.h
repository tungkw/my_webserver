#ifndef __M_CLIENT_H
#define __M_CLIENT_H
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include "http.h"
#include "process.h"

class Client{
public:
Client(int fd_):fd(fd_){}
~Client(){close(fd);}
bool read_data();

int fd;
std::string read_buffer;
std::string write_buffer;
HTTPRequest req;
HTTPResponse res;
};

#endif
