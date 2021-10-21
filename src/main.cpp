#include "server.h"

int main(){
    Server my_server = Server(10000, 1024, 60);
    my_server.start();
}