// basic socket setup from GeeksForGeeks
// https://www.geeksforgeeks.org/socket-programming-cc/

#include "server_socket_listener.hpp"

#include <thread>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <iostream>
#include <functional>
using std::function;
#include <chrono>


void ServerSocketListener::start_cleanup() {

    using namespace std::chrono_literals;

    this->cleanup_thread = new std::thread([=]() {

        while (this->is_listening()) {
        
            for (auto it = client_threads.begin(); it != client_threads.end(); ) {
                if (*it->is_finished) {
                    it->thread->join();
                    delete it->thread;
                    delete it->sock;
                    delete it->is_finished;
                    it = client_threads.erase(it);
                } else {
                    it++;
                }
            }

            std::this_thread::sleep_for(5000ms);

        }

    }); 

}


ServerSocketListener::ServerSocketListener(int port, function<void(ServerSocket*, bool*)> handler) {
    this->port = port;
    this->handler = handler;
    listen_thread = nullptr;
}


std::thread* ServerSocketListener::listen() {

    if (listen_thread != nullptr)
        return nullptr;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Error opening socket file descriptor" << std::endl;
        return nullptr;
    }

    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Error setting socket options" << std::endl;
        return nullptr;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(sock_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        std::cerr << "Error binding to port" << std::endl;
        return nullptr;
    }
    if (::listen(sock_fd, 10) < 0) {
        std::cerr << "Error listening on port" << std::endl;
        return nullptr;
    }

    auto listen_thread = new std::thread([=](ServerSocketListener *t) {
        while (true) {
            int new_sock;
            if ((new_sock = accept(t->sock_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
                std::cerr << "Error accepting client connection" << std::endl;
                return;
            }
            ServerSocket *conn = new ServerSocket(new_sock);
            bool *is_finished = new bool;
            *is_finished = false;
            std::thread *thread = new std::thread(handler, conn, is_finished);
            t->client_threads.push_back(ThreadKeeper { thread, conn, is_finished });
        }
    }, this);

    this->listen_thread = listen_thread;
    start_cleanup();
    return listen_thread;

}


bool ServerSocketListener::is_listening() {
    return listen_thread != nullptr;
}


std::thread* ServerSocketListener::get_listen_thread() {
    return listen_thread;
}


void ServerSocketListener::close() {
    if (listen_thread == nullptr)
        return;
    listen_thread->join();
    delete listen_thread;
    cleanup_thread->join();
    delete cleanup_thread;

    for (auto& t : client_threads) {
        t.thread->join();
    }

}

