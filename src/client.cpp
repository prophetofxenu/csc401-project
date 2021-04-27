#include "client_socket.hpp"
#include "server_socket.hpp"
#include "server_socket_listener.hpp"
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


void print_cli_help() {

    std::cout << "list              - display list of RFCs on index" << std::endl;
    std::cout << "get <rfc number>  - get an RFC by number" << std::endl;
    std::cout << "quit              - disconnect from index and quit the program" << std::endl;

}


bool add_available_rfcs(ClientSocket &sock, RFCManager &rfc_manager) {
    
    for (auto it : rfc_manager.get_rfcs()) {
        P2SMessage add_msg = P2SMessage::ADD;
        sock.send(&add_msg, sizeof(P2SMessage));
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
    ClientSocket sock(client.host, client.port);
    sock.connect();
    std::cout << "Connected to peer " << client.host << " (" << client.port
        << ")" << std::endl;
    // send hostname
    int tmp = my_host.length();
    sock.send(&tmp, sizeof(int));
    sock.send(const_cast<char*>(my_host.c_str()), tmp);
    // send upload port
    sock.send(&my_port, sizeof(int));

    // send request
    P2PMessage p2p_msg = P2PMessage::GET;
    sock.send(&p2p_msg, sizeof(P2PMessage));
    GetRFCMessage get_msg(rfc_num);
    std::byte *buf = get_msg.to_bytes();
    sock.send(buf, get_msg.message_size());
    delete[] buf;

    // get response
    sock.recv(&tmp, sizeof(int));
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

    p2p_msg = P2PMessage::DISCONNECT;
    sock.send(&p2p_msg, sizeof(P2PMessage));
    sock.disconnect();

    return true;

}


void print_rfc_list(const std::list<RFCHolder> &list) {

    std::cout << "| RFC  | Host            | Port  |" << std::endl;
    for (auto it : list) {
        std::cout << "| ";
        std::cout << std::setw(4) << it.rfc << " | ";
        std::cout << std::setw(15) << it.peer.host << " | ";
        std::cout << std::setw(5) << it.peer.port << " |" << std::endl;
    }

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

    auto handler = [&](ServerSocket *sock, bool *is_finished) {

        // get hostname
        int host_len;
        sock->recv(&host_len, sizeof(int));
        char host_c[host_len + 1];
        sock->recv(host_c, host_len);
        host_c[host_len] = '\0';
        std::string host(host_c);
        // get upload port
        int upload_port;
        sock->recv(&upload_port, sizeof(int));
        std::cout << "Connection from " << host << " (" << upload_port << ")" << std::endl;
        
        // get message
        P2PMessage message;
        do {
            sock->recv(&message, sizeof(P2PMessage));

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
                    int tmp = res.message_size();
                    sock->send(&tmp, sizeof(int));
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
    ServerSocketListener listener(port, handler);
    listener.listen();
    std::cout << "Server started" << std::endl;

    ClientSocket sock("127.0.0.1", 8080); 
    sock.connect();
    std::cout << "Connected to index" << std::endl;

    // send hostname
    int host_len = address.length();
    sock.send(&host_len, sizeof(int));
    sock.send(const_cast<char*>(address.c_str()), host_len);
    // send upload port
    sock.send(&port, sizeof(int));

    // add available RFCs
    add_available_rfcs(sock, rfc_manager);


    std::regex re_get("^get (\\d+)$");
    std::string input;
    do {
        std::cout << "\n> " << std::flush;
        std::getline(std::cin, input);
        std::cmatch cm;
        std::regex_match(input.c_str(), cm, re_get, std::regex_constants::match_default);

        if (cm.size() == 2) {

            int rfc_num = std::stoi(cm[1]);
            if (rfc_num <= 0) {
                std::cout << "RFC number must be positive integer" << std::endl;
                continue;
            }

            // lookup RFC
            P2SMessage p2s_message = P2SMessage::LOOKUP;
            sock.send(&p2s_message, sizeof(P2SMessage));
            LookupRFCMessage lookup_msg(rfc_num);
            std::byte *buf = lookup_msg.to_bytes();
            sock.send(buf, lookup_msg.message_size());
            delete[] buf;

            // get response
            int tmp;
            sock.recv(&tmp, sizeof(int));
            buf = new std::byte[tmp];
            sock.recv(buf, tmp);
            LookupRFCResponse lookup_res;
            lookup_res.from_bytes(buf);
            delete[] buf;
            if (lookup_res.get_code() == 404) {
                std::cout << "No nodes with RFC" << rfc_num << std::endl;
                continue;
            }

            Client peer = lookup_res.get_hosts().front();
            if (!get_from_peer(address, port, rfc_manager, rfc_num, peer))
                continue;

            // notify index
            p2s_message = P2SMessage::ADD;
            sock.send(&p2s_message, sizeof(P2SMessage));
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

        } else if (input == "list") {

            P2SMessage p2s_message = P2SMessage::LIST;
            sock.send(&p2s_message, sizeof(P2SMessage));

            // get response
            int tmp;
            sock.recv(&tmp, sizeof(int));
            std::byte *buf = new std::byte[tmp];
            sock.recv(buf, tmp);
            ListRFCResponse res;
            res.from_bytes(buf);
            delete[] buf;

            if (res.get_code() == 404) {
                std::cout << "No RFCs are in the index" << std::endl;
                continue;
            }

            print_rfc_list(res.get_holders());

        } else if (input == "help") {
            print_cli_help();
        } else if (input != "quit") {
            std::cout << "Invalid command" << std::endl;
        }

    } while (input != "quit");

    P2SMessage msg = P2SMessage::DISCONNECT;
    sock.send(&msg, sizeof(P2SMessage));
    sock.disconnect();

    return 0;
}

