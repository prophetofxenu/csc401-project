#include "server_socket.hpp"
#include "server_socket_listener.hpp"
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


int main(void) {

    auto handler = [](ServerSocket *sock, bool *is_finished) {
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
        std::cout << "  CONNECT - " << host << " (" << upload_port << ")" << std::endl;
        
        // add to CI
        CentralIndex::add_client(host, upload_port);

        // get message
        P2SMessage message;
        do {
            sock->recv(&message, sizeof(P2SMessage));

            switch (message) {

                case P2SMessage::ADD: {
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
                    std::cout << "      ADD - " << host << " (" << upload_port << ")"
                        << " RFC" << rfc_num << std::endl;
                    break;

                }

                case P2SMessage::LIST: {
                    list<RFCHolder> holders = CentralIndex::all_rfcs();
                    
                    int code = holders.size() != 0 ? 200 : 404;
                    ListRFCResponse res(code, holders);
                    std::byte *buf = res.to_bytes();
                    int tmp = res.message_size();
                    sock->send(&tmp, sizeof(int));
                    sock->send(buf, tmp);
                    delete[] buf;
                    std::cout << "     LIST - " << host << " (" << upload_port << ")" << std::endl;
                    break;

                }

                case P2SMessage::LOOKUP: {
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
                    sock->send(&tmp, sizeof(int));
                    sock->send(buf, tmp);
                    delete[] buf;
                    std::cout << "   LOOKUP - " << host << " (" << upload_port << ") RFC"
                        << rfc_num <<std::endl;
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

    };

    ServerSocketListener listener(8080, handler);
    std::cout << "Server started\n" << std::endl;
    auto thread = listener.listen();
    thread->join();

    return 0;

}

