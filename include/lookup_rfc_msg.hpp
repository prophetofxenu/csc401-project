#ifndef LOOKUP_RFC_MSG_H
#define LOOKUP_RFC_MSG_H

#include "abst_msg.hpp"

#include <cstddef>


class LookupRFCMessage : public AbstMessage {

private:
	std::string host;
	int port;
	std::string title;

public:
    LookupRFCMessage(void);
    LookupRFCMessage(std::string host, int port, std::string title);

    std::string get_host();
    void set_host(std::string host);
    int get_port();
    void set_port(int port);
    std::string get_title();
    void set_title(std::string title);

    bool is_valid();
    unsigned int message_size();
    void from_bytes(std::byte *bytes);
    std::byte* to_bytes();

};


#endif

