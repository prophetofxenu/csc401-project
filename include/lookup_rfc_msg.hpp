#ifndef LOOKUP_RFC_MSG_H
#define LOOKUP_RFC_MSG_H

#include "abst_msg.hpp"

#include <string>


class LookupRFCMessage : public AbstMessage {

private:
    int number;

public:
    LookupRFCMessage(void);
    LookupRFCMessage(int number);

    int get_number();
    void set_number(int number);

    bool is_valid();
    unsigned int message_size();
    void from_bytes(std::byte *bytes);
    std::byte* to_bytes();

};


#endif

