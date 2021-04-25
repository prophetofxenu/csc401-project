#ifndef LOOKUP_RFC_RES_H
#define LOOKUP_RFC_RES_H

#include "abst_msg.hpp"

#include <cstddef>


class LookupRFCResponse : public AbstMessage {

private:
	int code;
	int length;
	std::string* host;
	int* port;

public:
    LookupRFCResponse(void);
    LookupRFCResponse(int code, int length, std::string* host, int* port);

    int get_code();
    void set_code(int code);
    int get_length();
    void set_length(int length);
    std::string* get_host();
    void set_host(std::string* host);
    int* get_port();
    void set_port(int* port);

    bool is_valid();
    unsigned int message_size();
    void from_bytes(std::byte *bytes);
    std::byte* to_bytes();

};


#endif

