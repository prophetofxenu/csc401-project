#ifndef GET_P2P_req_H
#define GET_P2P_req_H

#include "abst_msg.hpp"

#include <cstddef>


class GetP2PRequest : public AbstMessage {

private:
	std::string host;
	std::string os;

public:
    GetP2PRequest(void);
    GetP2PRequest(std::string host, std::string os);

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

