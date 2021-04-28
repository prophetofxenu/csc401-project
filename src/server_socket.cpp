#include "server_socket.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#define MAX_READ 4096


ServerSocket::ServerSocket(int sock_fd) {
    this->sock_fd = sock_fd;
    connected = true;
}


bool ServerSocket::send(void *data, size_t len) {
    char *d = reinterpret_cast<char*>(data);
    if (!connected) {
        std::cerr << "Cannot send to disconnected socket" << std::endl;
    }
    int retries = 0;
    size_t sent = 0;
    while (retries < 10 && sent < len) {
        int s = ::send(sock_fd, d + sent, len - sent, 0);
        sent += s;
        if (s < 1)
            retries++;
    }
    if (retries == 10) {
        std::cerr << "Too many retries while sending bytes" << std::endl;
        return false;
    }
    return true;
}


bool ServerSocket::recv(void *buf, size_t len) {
    char *b = reinterpret_cast<char*>(buf);
    if (!connected) {
        std::cerr << "Cannot receive from disconnected socket" << std::endl;
    }
    int retries = 0;
    size_t received = 0;
    while (retries < 10 && received < len) {
        int r = ::recv(sock_fd, b + received, len - received, 0);
        received += r;
        if (r < 1)
            retries ++;
    }
    if (retries == 10) {
        std::cerr << "Too many retries while receiving bytes" << std::endl;
        return false;
    }
    return true;
}


bool ServerSocket::is_connected() {
    return connected;
}


void ServerSocket::close() {
    if (!connected)
        return;
    ::close(sock_fd);
}

