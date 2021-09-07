#include "server.h"

int main(){
    Server my_server = Server(1316, 1024);
    my_server.start();
}