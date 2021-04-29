#ifndef ABST_SERVER_SOCK_H
#define ABST_SERVER_SOCK_H

#include "abst_socket.hpp"

#include <cstddef>


class AbstServerSocket : public AbstSocket {

public:

    /* Send len bytes of data to client */
    virtual bool send(void *data, size_t len) = 0;

    /* Receive len bytes from client and store in buf */
    virtual bool recv(void *buf, size_t len) = 0;

    /* Tell if currently connected to client */
    virtual bool is_connected() = 0;

    /* Close socket */
    virtual void close() = 0;

};


#endif

