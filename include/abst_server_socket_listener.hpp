#ifndef ABST_SERVER_SOCK_LISTENER_H
#define ABST_SERVER_SOCK_LISTENER_H

#include <thread>


class AbstServerSocketListener {

public:

    /* Start listening at the port the socket was created with */
    virtual std::thread* listen() = 0;

    /* Report whether socket is listening for clients */
    virtual bool is_listening() = 0;

    /* Stop listening */
    virtual void close() = 0;

};


#endif

