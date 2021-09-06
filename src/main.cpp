#include "server.h"
//#include <iostream>

int main(){
    //std::cout << *((char*)(0)) << std::endl; 
    Server my_server = Server(1316, 1024);
    my_server.start();
}