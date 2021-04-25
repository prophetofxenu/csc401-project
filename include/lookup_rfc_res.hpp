#ifndef LOOKUP_RFC_RES_H
#define LOOKUP_RFC_RES_H

#include "abst_msg.hpp"

#include <cstddef>


class LookupRFCResponse : public AbstMessage {

private:
	int code;
	std::list<std::string> host;
	std::list<int> port;

public:
    LookupRFCResponse(void);
    LookupRFCResponse(int code, int length, std::list<std::string> host, std::list<int> port);

    int get_code();
    void set_code(int code);
    std::list<std::string> get_host();
    void set_host(std::list<std::string> host);
    std::list<int> get_port();
    void set_port(std::list<int> port);

    bool is_valid();
    unsigned int message_size();
    void from_bytes(std::byte *bytes);
    std::byte* to_bytes();

};


#endif

