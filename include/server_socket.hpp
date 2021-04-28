#ifndef SERVER_SOCK_H
#define SERVER_SOCK_H

#include "abst_server_socket.hpp"


class ServerSocket : public AbstServerSocket {

private:
    int sock_fd;
    bool connected;

public:
    
    ServerSocket(int sock_fd);
    virtual bool send(void *data, size_t len);
    virtual bool recv(void *buf, size_t len);
    virtual bool is_connected();
    virtual void close();

};


#endif

