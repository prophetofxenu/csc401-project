#include "server_socket.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#define MAX_READ 4096


ServerSocket::ServerSocket(int sock_fd) {
    this->sock_fd = sock_fd;
    connected = true;
}


/*
ServerSocket::~ServerSocket() {
    if (!connected)
        return;
    ::close(sock_fd);
}
*/


void ServerSocket::send(void *data, size_t len) {
    if (!connected) {
        std::cerr << "Cannot send to disconnected socket" << std::endl;
    }
    if (::send(sock_fd, data, len, 0) != len) {
        std::cerr << "Error sending bytes to client" << std::endl;
    }
}


void ServerSocket::recv(void *buf, size_t len) {
    if (!connected) {
        std::cerr << "Cannot receive from disconnected socket" << std::endl;
    }
    if (::recv(sock_fd, buf, len, 0) != len) {
        std::cerr << "Error receiving bytes from client" << std::endl;
    }
}


bool ServerSocket::is_connected() {
    return connected;
}


void ServerSocket::close() {
    if (!connected)
        return;
    ::close(sock_fd);
}

