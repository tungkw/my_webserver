#include "client.h"
#include <iostream>

bool Client::read_data(){
    char buff[1024];
    int len = 0;
    while(true){
        len = read(fd, buff, 1024);
        if(len > 0){
            read_buffer.append(&buff[0], (std::size_t)len);
        }
        else if(len == 0){
            return false;
        }
        else if(len == -1 && errno != EAGAIN){
            perror("read");
            std::cout << fd << std::endl;
            exit(-1);
        }
        else{
            break;
        }
    }
    return true;
}

bool Client::write_data(){
    int len = 0;
    while(true){
        len = write(fd, write_buffer.c_str(), write_buffer.length());
        if(len < 0) return false;
        else if(write_buffer.length() == len) break;
        else write_buffer = write_buffer.substr(len);
    }
    return true;
}

void Client::init(int fd_){fd = fd_; is_closed=false;}

void Client::close_fd(){
    if(!is_closed){
        is_closed = true;
        close(fd);
    }
}