#include "client_socket.hpp"

#include <iostream>


int main(void) {
    ClientSocket sock("127.0.0.1", 8080); 
    sock.connect();

    char *msg = "Hello from client";
    int len = 18;
    sock.send(&len, sizeof(int));
    sock.send(msg, len);

    sock.recv(&len, sizeof(int));
    char buf[len];
    sock.recv(buf, len);
    std::cout << buf << std::endl;

    return 0;
}

