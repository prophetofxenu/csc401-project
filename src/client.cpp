#include "encrypted_client_socket.hpp"
#include "encrypted_server_socket.hpp"
#include "encrypted_server_socket_listener.hpp"
#include "common.hpp"
#include "add_rfc_msg.hpp"
#include "add_rfc_res.hpp"
#include "get_rfc_msg.hpp"
#include "get_rfc_res.hpp"
#include "list_rfc_res.hpp"
#include "lookup_rfc_msg.hpp"
#include "lookup_rfc_res.hpp"
#include "rfc_manager.hpp"

#include <iostream>
#include <iomanip>
#include <regex>
#include <functional>
#include <csignal>
#include <arpa/inet.h>


void print_cli_help() {

    std::cout << "list              - display list of RFCs on index" << std::endl;
    std::cout << "listl             - display list of local RFCs " << std::endl;
    std::cout << "get <rfc number>  - get an RFC by number" << std::endl;
    std::cout << "quit              - disconnect from index and quit the program" << std::endl;

}


bool add_available_rfcs(ClientSocket &sock, RFCManager &rfc_manager) {
    
    for (auto it : rfc_manager.get_rfcs()) {
        int tmp = htonl(static_cast<int>(P2SMessage::ADD));
        sock.send(&tmp, sizeof(P2SMessage));
        AddRFCMessage msg(it);
        std::byte *buf = msg.to_bytes();
        sock.send(buf, sizeof(int));
        delete[] buf;
        
        buf = new std::byte[sizeof(int)];
        sock.recv(buf, sizeof(int));
        AddRFCResponse res;
        res.from_bytes(buf);
        delete[] buf;
        if (res.get_code() != 200) {
            std::cout << "Error when adding RFC" << it << " to CI" << std::endl;
            return false;
        }
    }

    return true;

}


bool get_from_peer(std::string &my_host, int my_port, RFCManager &rfc_manager,
        int rfc_num, Client &client) {

    // connect to peer
    EncryptedClientSocket sock(client.host, client.port);
    sock.connect();
    std::cout << "Connected to peer " << client.host << " (" << client.port
        << ")" << std::endl;
    // send hostname
    int tmp = my_host.length();
    tmp = htonl(tmp);
    sock.send(&tmp, sizeof(int));
    tmp = ntohl(tmp);
    sock.send(const_cast<char*>(my_host.c_str()), tmp);
    // send upload port
    tmp = htonl(my_port);
    sock.send(&tmp, sizeof(int));

    // send request
    tmp = htonl(static_cast<int>(P2PMessage::GET));
    sock.send(&tmp, sizeof(P2PMessage));
    GetRFCMessage get_msg(rfc_num);
    std::byte *buf = get_msg.to_bytes();
    sock.send(buf, get_msg.message_size());
    delete[] buf;

    // get response
    sock.recv(&tmp, sizeof(int));
    tmp = ntohl(tmp);
    buf = new std::byte[tmp];
    sock.recv(buf, tmp);
    GetRFCResponse get_res;
    get_res.from_bytes(buf);
    delete[] buf;

    if (get_res.get_code() == 404) {
        std::cout << "Peer returned 404 for RFC" << rfc_num << std::endl;
        sock.disconnect();
        return false;
    }

    std::string content = get_res.get_content();
    rfc_manager.add_rfc(rfc_num, content);
    std::cout << "Download of RFC" << rfc_num << " complete" << std::endl;

    // disconnect

    tmp = htonl(static_cast<int>(P2PMessage::DISCONNECT));
    sock.send(&tmp, sizeof(P2PMessage));
    sock.disconnect();

    return true;

}


void print_rfc_list(const std::list<RFCHolder> &list, std::string &host, int upload_port) {

    std::cout << "| RFC  | Host            | Port  |" << std::endl;
    for (auto it : list) {
        std::cout << "| ";
        std::cout << std::setw(4) << it.rfc << " | ";
        std::cout << std::setw(15) << it.peer.host << " | ";
        std::cout << std::setw(5) << it.peer.port << " |";
        if (it.peer.host == host && it.peer.port == upload_port)
            std::cout << " (this node)";
        std::cout << std::endl;
    }

}


bool check_if_rfc_stored(int rfc, RFCManager &rfc_manager) {
    for (auto it : rfc_manager.get_rfcs()) {
        if (rfc == it)
            return true;
    }
    return false;
}


void get_rfc(ClientSocket &sock, int rfc_num, std::string &my_host, int my_port,
        RFCManager &rfc_manager) {

    if (rfc_num <= 0) {
        std::cout << "RFC number must be positive integer" << std::endl;
        return;
    }

    // check if already have rfc
    if (check_if_rfc_stored(rfc_num, rfc_manager)) {
        std::cout << "Already have RFC" << rfc_num << std::endl;
        return;
    }

    // lookup RFC
    int tmp = htonl(static_cast<int>(P2SMessage::LOOKUP));
    sock.send(&tmp, sizeof(P2SMessage));
    LookupRFCMessage lookup_msg(rfc_num);
    std::byte *buf = lookup_msg.to_bytes();
    sock.send(buf, lookup_msg.message_size());
    delete[] buf;

    // get response
    sock.recv(&tmp, sizeof(int));
    tmp = ntohl(tmp);
    buf = new std::byte[tmp];
    sock.recv(buf, tmp);
    LookupRFCResponse lookup_res;
    lookup_res.from_bytes(buf);
    delete[] buf;
    if (lookup_res.get_code() == 404) {
        std::cout << "No nodes with RFC" << rfc_num << std::endl;
        return;
    }

    Client peer = lookup_res.get_hosts().front();
    if (!get_from_peer(my_host, my_port, rfc_manager, rfc_num, peer))
        return;

    // notify index
    tmp = htonl(static_cast<int>(P2SMessage::ADD));
    sock.send(&tmp, sizeof(P2SMessage));
    AddRFCMessage add_msg(rfc_num);
    buf = add_msg.to_bytes();
    sock.send(buf, add_msg.message_size());
    delete[] buf;

    buf = new std::byte[sizeof(int)];
    sock.recv(buf, sizeof(int));
    AddRFCResponse add_res;
    add_res.from_bytes(buf);
    delete[] buf;
    if (add_res.get_code() != 200) {
        std::cout << "Error notifying index" << std::endl;
    }

}


void list_rfcs_remote(ClientSocket &sock, std::string &host, int upload_port) {

    int tmp = htonl(static_cast<int>(P2SMessage::LIST));
    sock.send(&tmp, sizeof(P2SMessage));

    // get response
    sock.recv(&tmp, sizeof(int));
    tmp = ntohl(tmp);
    std::byte *buf = new std::byte[tmp];
    sock.recv(buf, tmp);
    ListRFCResponse res;
    res.from_bytes(buf);
    delete[] buf;

    if (res.get_code() == 404) {
        std::cout << "No RFCs are in the index" << std::endl;
        return;
    }

    print_rfc_list(res.get_holders(), host, upload_port);

}


void list_rfcs_local(RFCManager &rfc_manager) {

    std::list<int> rfcs = rfc_manager.get_rfcs();
    if (rfcs.size() == 0) {
        std::cout << "No RFCs stored locally" << std::endl;
        return;
    }
    std::cout << "| RFC  |" << std::endl;
    for (auto it : rfcs) {
        std::cout << "| " << std::setw(4) << it << " |" << std::endl;
    }

}


void disconnect(ClientSocket& sock) {
    int tmp = htonl(static_cast<int>(P2SMessage::DISCONNECT));
    sock.send(&tmp, sizeof(P2SMessage));
    sock.disconnect();
}


int main(int argc, char *argv[]) {

    if (argc != 4) {
        std::cout << "usage: client <rfc_dir> <address> <upload_port>" << std::endl;
        return 0;
    }

    // set alignment to left
    std::cout << std::left;

    std::string rfc_dir(argv[1]);
    std::string address(argv[2]);
    int port = std::stoi(argv[3]);

    RFCManager rfc_manager(rfc_dir);

    // handler for connections from other peers
    // defined here as lambda so we can use rfc_manager
    auto handler = [&](ServerSocket *sock, bool *is_finished) {

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
        std::cout << "Connection from " << host << " (" << upload_port << ")" << std::endl;
        
        // get message
        P2PMessage message;
        do {
            sock->recv(&message, sizeof(P2PMessage));
            message = static_cast<P2PMessage>(ntohl(static_cast<int>(message)));

            switch (message) {
                case P2PMessage::GET: {
                    std::byte *buf = new std::byte[sizeof(short)];
                    sock->recv(buf, sizeof(short));
                    GetRFCMessage msg;
                    msg.from_bytes(buf);
                    delete[] buf;

                    short rfc_num = msg.get_number();
                    std::string content("");
                    int code;
                    try {
                        content = rfc_manager.get_content(rfc_num);
                        code = 200;
                        std::cout << "Sending RFC" << rfc_num << " to " << host << " ("
                            << upload_port << ")" << std::endl;
                    } catch (const FileReadException& e) {
                        code = 404;
                        std::cout << host << " (" << upload_port << ") requested RFC"
                            << rfc_num << " which is not saved" << std::endl;
                    }
                    
                    GetRFCResponse res(code, content);
                    buf = res.to_bytes();
                    int tmp = htonl(res.message_size());
                    sock->send(&tmp, sizeof(int));
                    tmp = ntohl(tmp);
                    sock->send(buf, tmp);
                    delete[] buf;

                    break;
                }

                default:
                    break;
            }

        } while (message != P2PMessage::DISCONNECT);

        *is_finished = true;
        
    };

    // start listener
    EncryptedServerSocketListener listener(port, handler);
    listener.listen();
    std::cout << "Server started" << std::endl;

    EncryptedClientSocket sock("127.0.0.1", 8080); 
    sock.connect();
    std::cout << "Connected to index" << std::endl;

    // get and send protocol version
    int client_version = htonl(PROTOCOL_VERSION);
    sock.send(&client_version, sizeof(int));
    client_version = ntohl(client_version);
    int server_version;
    sock.recv(&server_version, sizeof(int));
    server_version = ntohl(server_version);
    if (server_version != client_version) {
        std::cout << "Tried connecting, but server uses unsupported protocol version"
            << std::endl;
        return 1;
    }

    // send hostname
    int host_len = address.length();
    host_len = htonl(host_len);
    sock.send(&host_len, sizeof(int));
    host_len = ntohl(host_len);
    sock.send(const_cast<char*>(address.c_str()), host_len);
    // send upload port
    int tmp = htonl(port);
    sock.send(&tmp, sizeof(int));

    // add available RFCs
    add_available_rfcs(sock, rfc_manager);

    // regex for parsing get command
    std::regex re_get("^get (\\d+)$");
    std::string input;
    do {
        std::cout << "\n> " << std::flush;
        std::getline(std::cin, input);
        std::cmatch cm;
        std::regex_match(input.c_str(), cm, re_get, std::regex_constants::match_default);

        if (cm.size() == 2) {

            int rfc_num = std::stoi(cm[1]);
            get_rfc(sock, rfc_num, address, port, rfc_manager);

        } else if (input == "list") {

            list_rfcs_remote(sock, address, port);

        } else if (input == "listl") {

            list_rfcs_local(rfc_manager);

        } else if (input == "help") {
            print_cli_help();
        } else if (input != "quit") {
            std::cout << "Invalid command" << std::endl;
        }

    } while (input != "quit");

    disconnect(sock);

    return 0;
}

