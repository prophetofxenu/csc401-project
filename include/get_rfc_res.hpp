#ifndef GET_RFC_RES_H
#define GET_RFC_RES_H

#include "abst_msg.hpp"

#include <string>


class GetRFCResponse : public AbstMessage {

private:
    int code;
    std::string content;

public:
    GetRFCResponse(void);
    GetRFCResponse(int code, std::string content);

    int get_code();
    void set_code(int code);
    std::string get_content();
    void set_content(std::string content);

    bool is_valid();
    unsigned int message_size();
    void from_bytes(std::byte *bytes);
    std::byte* to_bytes();

};


#endif

