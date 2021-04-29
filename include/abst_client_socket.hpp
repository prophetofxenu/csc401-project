#ifndef ABST_CLIENT_SOCK_H
#define ABST_CLIENT_SOCK_H

#include "abst_socket.hpp"

#include <cstddef>


class AbstClientSocket : public AbstSocket {

public:

    /* Connect to the address the socket has been created with */
    virtual void connect() = 0;

    /* Disconnect from the server */
    virtual void disconnect() = 0;

    /* Tell if socket is currently connected to server */
    virtual bool is_connected() = 0;

    /* Send len bytes stored in data to the server */
    virtual bool send(void *data, size_t len) = 0;

    /* Receive len bytes from the server and store in buf */
    virtual bool recv(void *buf, size_t len) = 0;

};


#endif

