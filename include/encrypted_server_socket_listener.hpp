#ifndef CRYPT_SERVER_SOCK_LISTENER_H
#define CRYPT_SERVER_SOCK_LISTENER_H

#include "abst_server_socket_listener.hpp"
#include "encrypted_server_socket.hpp"


struct ThreadKeeper {
    std::thread *thread;
    EncryptedServerSocket *sock;
    bool *is_finished;
};


class EncryptedServerSocketListener : public AbstServerSocketListener {

private:
    int sock_fd;
    int port;

    std::function<void(EncryptedServerSocket*, bool*)> handler;

    std::thread *listen_thread;
    std::thread *cleanup_thread;
    std::vector<ThreadKeeper> client_threads;

    void start_cleanup();

public:
    
    EncryptedServerSocketListener(int port,
            std::function<void(EncryptedServerSocket*, bool*)> handler);
    virtual std::thread* listen();
    virtual bool is_listening();
    std::thread* get_listen_thread();
    virtual void close();

};


#endif

