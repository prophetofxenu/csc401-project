#ifndef LIST_RFC_MSG_H
#define LIST_RFC_MSG_H

#include "abst_msg.hpp"

#include <cstddef>


class ListRFCMessage : public AbstMessage {

private:
	std::string host;
	int port;

public:
    ListRFCMessage(void);
    ListRFCMessage(std::string host, int port);

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

