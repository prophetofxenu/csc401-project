#include "server_socket.hpp"
#include "server_socket_listener.hpp"

#include <cstdlib>
#include <iostream>


int main(void) {
    auto handler = [](ServerSocket *sock) {
        int len;
        sock->recv(&len, sizeof(int));
        char buf[len];
        sock->recv(buf, len);
        std::cout << buf << std::endl;

        char *msg = "Hello from server";
        len = 18;
        sock->send(&len, sizeof(int));
        sock->send(msg, len);
        exit(EXIT_SUCCESS);
    };

    ServerSocketListener listener(8080, handler);
    auto thread = listener.listen();
    thread->join();
}

