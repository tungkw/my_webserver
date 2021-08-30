#include "server.h"

int main(){
    Server my_server = Server(10000, 1024);
    my_server.start();
}