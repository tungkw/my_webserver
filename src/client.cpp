#include "client.h"
#include <iostream>

bool Client::read_data(){
    char buff[10];
    while(true){
        int len = read(fd, buff, 10);
        if(len > 0){
            for(int i = 0; i<len; i++)
                read_buffer.append(&buff[0], (std::size_t)len);
        }
        else if(len == 0){
            return false;
        }
        else if(len == -1 && errno != EAGAIN){
            perror("read");
            exit(-1);
        }
        else{
            break;
        }
    }
    return true;
}

