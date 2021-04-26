#ifndef ADD_RFC_MSG_H
#define ADD_RFC_MSG_H

#include "abst_msg.hpp"

#include <string>


class AddRFCMessage : public AbstMessage {

private:
    std::string host;
    int port;
    int number;

public:
    AddRFCMessage(void);
    AddRFCMessage(std::string host, int port, int number);
    ~AddRFCMessage();

    std::string get_host();
    void set_host(std::string host);
    int get_port();
    void set_port(int port);
    int get_number();
    void set_number(int number);

    bool is_valid();
    unsigned int message_size();
    void from_bytes(std::byte *bytes);
    std::byte* to_bytes();

};


#endif

