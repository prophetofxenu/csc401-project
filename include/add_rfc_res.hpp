#ifndef ADD_RFC_RES_H
#define ADD_RFC_RES_H

#include "abst_msg.hpp"

#include <string>


class AddRFCResponse : public AbstMessage {

private:
    int code;

public:
    AddRFCResponse(void);
    AddRFCResponse(int code);

    int get_code();
    void set_code(int code);

    bool is_valid();
    unsigned int message_size();
    void from_bytes(std::byte *bytes);
    std::byte* to_bytes();

};


#endif

