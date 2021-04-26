#ifndef LOOKUP_RFC_MSG_H
#define LOOKUP_RFC_MSG_H

#include "abst_msg.hpp"

#include <string>


class LookupRFCMessage : public AbstMessage {

private:
    int number;
    std::string host;
    int port;

public:
    LookupRFCMessage(void);
    LookupRFCMessage(int number, std::string host, int port);

    int get_number();
    void set_number(int number);
    std::string get_host();
    void set_host(std::string host);
    int get_port();
    void set_port(int port);

    bool is_valid();
    unsigned int message_size();
    void from_bytes(std::byte *bytes);
    std::byte* to_bytes();

};


#endif

