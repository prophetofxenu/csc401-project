#ifndef GET_RFC_MSG_H
#define GET_RFC_MSG_H

#include "abst_msg.hpp"

#include <cstddef>


class GetRFCMessage : public AbstMessage {

private:
    int lengthHost;
    int lengthOs;
	std::string host;
	std::string os;

public:
    GetP2PRequest(void);
    GetP2PRequest(short number);

    int get_lengthHost();
    void set_lengthHost(int length);
    int get_lengthOs();
    void set_lengthOs(int length);
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

