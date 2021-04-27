#ifndef LIST_RFC_RES_H
#define LIST_RFC_RES_H

#include "abst_msg.hpp"

#include "common.hpp"

#include <list>
#include <string>


class ListRFCResponse : public AbstMessage {

private:
    int code;
    std::list<RFCHolder> holders;

public:
    ListRFCResponse(void);
    ListRFCResponse(int code, std::list<RFCHolder> holders);

    int get_code();
    void set_code(int code);
    std::list<RFCHolder> get_holders();
    void set_holders(std::list<RFCHolder> holders);

    bool is_valid();
    unsigned int message_size();
    void from_bytes(std::byte *bytes);
    std::byte* to_bytes();

};


#endif

