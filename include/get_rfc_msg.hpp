#ifndef GET_RFC_MSG_H
#define GET_RFC_MSG_H

#include "abst_msg.hpp"

#include <cstddef>


class GetRFCMessage : public AbstMessage {

private:
    short number;
	std::string host;
	std::string os;

public:
    GetRFCMessage(void);
    GetRFCMessage(short number, std::string host, std::string os);

    short get_number();
    void set_number(short number);
    std::string get_host();
    void set_host(std::string host);
    std::string get_os();
    void set_os(sstd::tring os);

    bool is_valid();
    unsigned int message_size();
    void from_bytes(std::byte *bytes);
    std::byte* to_bytes();

};


#endif

