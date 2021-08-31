#include <iostream>
#include <arpa/inet.h>
//#include <sys/socket.h>
//#include <fcntl.h>
#include <unistd.h>

int main(){
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(10000);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
    //inet_pton(AF_INET, "192.168.234.118", &addr.sin_addr.s_addr);
    int ret = connect(fd, (sockaddr*)&addr, sizeof(addr));
    if(ret == -1){
        perror("connect");
        exit(-1);
        
    }

    std::cout << "connected" << std::endl;
    char buff[6] = "hello";
    while(true){
        write(fd, buff, 6);
        sleep(3);
    }
}