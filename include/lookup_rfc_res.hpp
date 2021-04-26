#ifndef LOOKUP_RFC_RES_H
#define LOOKUP_RFC_RES_H

#include "abst_msg.hpp"
#include "common.hpp"

#include <list>


class LookupRFCResponse : public AbstMessage {

private:
    int code;
    std::list<Client> hosts;

public:
    LookupRFCResponse(void);
    LookupRFCResponse(int code, std::list<Client> hosts);

    int get_code();
    void set_code(int code);
    std::list<Client> get_hosts();
    void set_hosts(std::list<Client> hosts);

    bool is_valid();
    unsigned int message_size();
    void from_bytes(std::byte *bytes);
    std::byte* to_bytes();

};


#endif

