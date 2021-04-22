#ifndef SERVER_SOCK_H
#define SERVER_SOCK_H

#include "abst_server_socket.hpp"

#include <vector>
#include <thread>


class ServerSocket : public AbstServerSocket {

private:
    int sock_fd;
    bool connected;

public:
    
    ServerSocket(int sock_fd);
    //virtual ~ServerSocket();
    virtual void send(void *data, size_t len);
    virtual void recv(void *buf, size_t len);
    virtual bool is_connected();
    virtual void close();

};


#endif

