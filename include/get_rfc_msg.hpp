#ifndef GET_RFC_MSG_H
#define GET_RFC_MSG_H

#include "abst_msg.hpp"

#include <string>


class GetRFCMessage : public AbstMessage {

private:
    short number;

public:
    GetRFCMessage(void);
    GetRFCMessage(short number);

    short get_number();
    void set_number(short number);

    bool is_valid();
    unsigned int message_size();
    void from_bytes(std::byte *bytes);
    std::byte* to_bytes();

};


#endif

