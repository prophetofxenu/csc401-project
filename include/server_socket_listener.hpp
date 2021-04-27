#ifndef SERVER_SOCK_LISTENER_H
#define SERVER_SOCK_LISTENER_H

#include "abst_server_socket_listener.hpp"

#include "server_socket.hpp"

#include <thread>
#include <netinet/in.h>
#include <functional>


struct ThreadKeeper {
    std::thread *thread;
    ServerSocket *sock;
    bool *is_finished;
};


class ServerSocketListener : public AbstServerSocketListener {

private:

    int sock_fd;
    int port;

    std::function<void(ServerSocket*, bool*)> handler;

    std::thread *listen_thread;
    std::thread *cleanup_thread;
    std::vector<ThreadKeeper> client_threads;

    void start_cleanup();

public:
    
    ServerSocketListener(int port, std::function<void(ServerSocket*, bool*)> handler);
    virtual std::thread* listen();
    virtual bool is_listening();
    std::thread* get_listen_thread();
    virtual void close();

};


#endif

