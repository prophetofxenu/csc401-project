#ifndef SERVER_SOCK_LISTENER_H
#define SERVER_SOCK_LISTENER_H

#include "abst_server_socket_listener.hpp"

#include "server_socket.hpp"

#include <thread>
#include <netinet/in.h>
#include <functional>


class ServerSocketListener : public AbstServerSocketListener {

private:

    int sock_fd;
    int port;

    std::function<void(ServerSocket*)> handler;

    std::thread *listen_thread;
    std::vector<std::thread*> client_threads;
    std::vector<ServerSocket*> client_sockets;

public:
    
    ServerSocketListener(int port, std::function<void(ServerSocket*)> handler);
    virtual std::thread* listen();
    virtual bool is_listening();
    std::thread* get_listen_thread();
    virtual void close();

};


#endif

