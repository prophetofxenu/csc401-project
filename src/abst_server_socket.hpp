#ifndef ABST_SERVER_SOCK_H
#define ABST_SERVER_SOCK_H

#include <cstddef>


class AbstServerSocket {

public:

    /* Send len bytes of data to client */
    virtual void send(void *data, size_t len) = 0;

    /* Receive len bytes from client and store in buf */
    virtual void recv(void *buf, size_t len) = 0;

    /* Tell if currently connected to client */
    virtual bool is_connected() = 0;

};


#endif

