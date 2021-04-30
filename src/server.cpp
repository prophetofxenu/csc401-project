#include "encrypted_server_socket.hpp"
#include "encrypted_server_socket_listener.hpp"
#include "central_index.hpp"
#include "common.hpp"
#include "add_rfc_msg.hpp"
#include "add_rfc_res.hpp"
#include "list_rfc_res.hpp"
#include "lookup_rfc_msg.hpp"
#include "lookup_rfc_res.hpp"

#include <cstdlib>
#include <iostream>
#include <list>
using std::list;
#include <arpa/inet.h>


void add_rfc(ServerSocket *sock, std::string &host, int upload_port) {

    std::byte *buf = new std::byte[sizeof(int)];
    sock->recv(buf, sizeof(int));
    AddRFCMessage msg;
    msg.from_bytes(buf);
    delete[] buf;

    int rfc_num = msg.get_number();
    int code = 200;
    if (!CentralIndex::add_rfc(host, upload_port, rfc_num))
        code = 400;
    
    AddRFCResponse res(code);
    buf = res.to_bytes();
    sock->send(buf, res.message_size());
    delete[] buf;
    std::cout << "       ADD - " << host << " (" << upload_port << ")"
        << " RFC" << rfc_num << std::endl;

}


void list_rfcs(ServerSocket *sock, std::string &host, int upload_port) {

    list<RFCHolder> holders = CentralIndex::all_rfcs();
    
    int code = holders.size() != 0 ? 200 : 404;
    ListRFCResponse res(code, holders);
    std::byte *buf = res.to_bytes();
    int tmp = res.message_size();
    tmp = htonl(tmp);
    sock->send(&tmp, sizeof(int));
    tmp = ntohl(tmp);
    sock->send(buf, tmp);
    delete[] buf;
    std::cout << "      LIST - " << host << " (" << upload_port << ")" << std::endl;

}


void lookup_rfc(ServerSocket *sock, std::string &host, int upload_port) {

    std::byte *buf = new std::byte[sizeof(int)];
    sock->recv(buf, sizeof(int));
    LookupRFCMessage msg;
    msg.from_bytes(buf);
    delete[] buf;

    int rfc_num = msg.get_number();
    list<Client> clients = CentralIndex::find_rfcs(rfc_num);
    int code = 200;
    if (clients.size() == 0)
        code = 404;

    LookupRFCResponse res(code, clients);
    buf = res.to_bytes();
    int tmp = res.message_size();
    tmp = htonl(tmp);
    sock->send(&tmp, sizeof(int));
    tmp = ntohl(tmp);
    sock->send(buf, tmp);
    delete[] buf;
    std::cout << "    LOOKUP - " << host << " (" << upload_port << ") RFC"
        << rfc_num <<std::endl;

}


void client_handler(ServerSocket *sock, bool *is_finished) {

    // get and send protocol version
    int server_version = htonl(PROTOCOL_VERSION);
    sock->send(&server_version, sizeof(int));
    server_version = ntohl(server_version);
    int client_version;
    sock->recv(&client_version, sizeof(int));
    client_version = ntohl(client_version);
    if (server_version != client_version) {
        std::cout << "Client tried connecting, but uses unsupported protocol version"
            << std::endl;
        *is_finished = false;
        return;
    }

    // get hostname
    int host_len;
    sock->recv(&host_len, sizeof(int));
    host_len = ntohl(host_len);
    char host_c[host_len + 1];
    sock->recv(host_c, host_len);
    host_c[host_len] = '\0';
    std::string host(host_c);
    // get upload port
    int upload_port;
    sock->recv(&upload_port, sizeof(int));
    upload_port = ntohl(upload_port);
    std::cout << "   CONNECT - " << host << " (" << upload_port << ")" << std::endl;
    
    // add to CI
    CentralIndex::add_client(host, upload_port);

    // get message
    P2SMessage message;
    do {
        sock->recv(&message, sizeof(P2SMessage));
        message = static_cast<P2SMessage>(ntohl(static_cast<int>(message)));

        switch (message) {

            case P2SMessage::ADD: {

                add_rfc(sock, host, upload_port);
                break;

            }

            case P2SMessage::LIST: {

                list_rfcs(sock, host, upload_port);
                break;

            }

            case P2SMessage::LOOKUP: {

                lookup_rfc(sock, host, upload_port);
                break;

            }

            default:
                break;

        }

    } while (message != P2SMessage::DISCONNECT);

    // remove from CI
    CentralIndex::remove_client(host, upload_port);
    std::cout << "DISCONNECT - " << host << " (" << upload_port << ")" << std::endl;
    sock->close();

    *is_finished = true;

}


int main(void) {

    EncryptedServerSocketListener listener(8080, client_handler);
    std::cout << "Server started\n" << std::endl;
    auto thread = listener.listen();
    thread->join();

    return 0;

}

