// basic socket setup from GeeksForGeeks
// https://www.geeksforgeeks.org/socket-programming-cc/

#include "client_socket.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
using std::string;
#include <iostream>


ClientSocket::ClientSocket(string addr, uint16_t port) {
    this->connected = false;
    this->addr = addr;
    this->port = port;
}


void ClientSocket::connect() {
    if (connected)
        return;

    struct sockaddr_in serv_addr;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Unable to open client sock file descriptor" << std::endl;
        return;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, addr.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address: " << addr << std::endl;
        return;
    }
    if (::connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Unable to connect to server" << std::endl;
        return;
    }
    connected = true;
}


void ClientSocket::disconnect() {
    if (!connected)
        return;

    if (close(sock_fd) < 0) {
        std::cerr << "Unable to close socket" << std::endl;
        return;
    }
    connected = false;
}


bool ClientSocket::is_connected() {
    return connected;
}


bool ClientSocket::send(void *data, size_t len) {
    char *d = reinterpret_cast<char*>(data);
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


bool ClientSocket::recv(void *buf, size_t len) {
    char *b = reinterpret_cast<char*>(buf);
    int retries = 0;
    size_t received = 0;
    while (received < len) {
        int r = ::recv(sock_fd, b + received, len - received, 0);
        received += r;
        if (r < 1)
            retries++;
    }
    if (retries == 10) {
        std::cerr << "Too many retries while receiving bytes" << std::endl;
        return false;
    }
    return true;
}

