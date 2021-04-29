#ifndef ABST_SOCK_H
#define ABST_SOCK_H

#include <cstddef>


class AbstSocket {

public:

    /* Send len bytes stored in data to the other side */
    virtual bool send(void *data, size_t len) = 0;

    /* Receive len bytes from the other side and store in buf */
    virtual bool recv(void *buf, size_t len) = 0;

};


#endif

