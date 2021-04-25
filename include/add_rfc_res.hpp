#ifndef ADD_RFC_RES_H
#define ADD_RFC_RES_H

#include "abst_msg.hpp"

#include <cstddef>


class AddRFCResponse : public AbstMessage {

private:
	int code;
	std::string host;
	int port;

public:
    AddRFCResponse(void);
    AddRFCResponse(int code, std::string host, int port);

    int get_code();
    void set_code(int code);
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

