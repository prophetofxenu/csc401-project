#include "get_rfc_msg.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


GetRFCMessage::GetRFCMessage(void) : number(0) {}


GetRFCMessage::GetRFCMessage(short number) : number(number) {}


short GetRFCMessage::get_number() {
    return number;
}


void GetRFCMessage::set_number(short number) {
    this->number = number;
}

bool GetRFCMessage::is_valid() {
    // check that all fields are either positive or not empty
    return number > 0;
}


unsigned int GetRFCMessage::message_size() {
    return sizeof(short);
}


void GetRFCMessage::from_bytes(std::byte *bytes) {
    // copy 2 bytes (size of short) from byte sequence to number
    std::memcpy(&number, bytes, sizeof(short));
}


std::byte* GetRFCMessage::to_bytes() {
    if (!is_valid())
        return nullptr;
    // allocate a new buffer to store the message in
    std::byte *buf = new std::byte[message_size()];
    // copy 2 bytes (size of short) from number to buffer
    std::memcpy(buf, &number, sizeof(short));
    return buf;
}

