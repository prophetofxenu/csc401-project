#ifndef LIST_RFC_RES_H
#define LIST_RFC_RES_H

#include "abst_msg.hpp"

#include <list>
#include <string>


struct Client {
    std::string host;
    int port;
};


class ListRFCResponse : public AbstMessage {

private:
    int code;
    std::list<Client> hosts;

public:
    ListRFCResponse(void);
    ListRFCResponse(int code, std::list<Client> hosts);

    int get_code();
    void set_code(int code);
    std::list<Client> get_hosts();
    void set_hosts(std::list<Client> hosts);

    bool is_valid();
    unsigned int message_size();
    void from_bytes(std::byte *bytes);
    std::byte* to_bytes();

};


#endif

