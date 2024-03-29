#include "add_rfc_res.hpp"

#include <cstddef>
using std::byte;
#include <cstring>
#include <arpa/inet.h>


AddRFCResponse::AddRFCResponse(void) : code(0) {}


AddRFCResponse::AddRFCResponse(int code) : code(code) {}


int AddRFCResponse::get_code() {
    return code;
}


void AddRFCResponse::set_code(int code) {
    this->code = code;
}


bool AddRFCResponse::is_valid() {
    // check that all fields are either positive or not empty
    if ( code != 200 && code != 400 && code != 404 && code != 505 ) {
        return false;
    }
    return true;
}


unsigned int AddRFCResponse::message_size() {
    return sizeof(int);
}


void AddRFCResponse::from_bytes(std::byte *bytes) {
    // copy int bytes from byte sequence to length to find code
    std::memcpy(&code, bytes, sizeof(int));
    code = ntohl(code);
}


std::byte* AddRFCResponse::to_bytes() {
    if (!is_valid())
        return nullptr;
    // allocate a new buffer to store the message in
    std::byte *buf = new std::byte[message_size()];
    // copy int bytes to buffer
    int tmp = htonl(code);
    std::memcpy(buf, &tmp, sizeof(int));
    return buf;
}

