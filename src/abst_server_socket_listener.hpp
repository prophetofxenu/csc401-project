#ifndef ABST_SERVER_SOCK_LISTENER_H
#define ABST_SERVER_SOCK_LISTENER_H


class AbstServerSocketListener {

public:

    /* Start listening at the port the socket was created with */
    virtual void listen() = 0;

    /* Stop listening */
    virtual void close() = 0;

};


#endif

