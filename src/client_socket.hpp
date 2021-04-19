#ifndef CLIENT_SOCK_H
#define CLIENT_SOCK_H

#include "abst_client_socket.hpp"

#include <cstdint>
#include <string>
using std::string;


class ClientSocket : public AbstClientSocket {

private:
    bool connected;
    unsigned int sock_fd;
    string addr;
    uint16_t port;

public:
    ClientSocket(string addr, uint16_t port);
    virtual void connect();
    virtual void disconnect();
    virtual bool is_connected();
    virtual void send(void *data, size_t len);
    virtual void recv(void *buf, size_t len);

};


#endif

