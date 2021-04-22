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
    // all we have is a short so this will always be valid
    return true;
}


unsigned int GetRFCMessage::message_size() {
    return sizeof(short);
}


void GetRFCMessage::from_bytes(std::byte *bytes) {
    // use pos to keep track of where we are in the byte sequence
    unsigned int pos = 0;
    // copy 2 bytes (size of short) from byte sequence to number
    std::memcpy(&number, bytes + pos, sizeof(short));
    // move pos forward 2 bytes, the amount we just copied
    // there's no point here since there's nothing left to copy, it just serves
    // as an example
    pos += sizeof(short);
}


std::byte* GetRFCMessage::to_bytes() {
    // allocate a new buffer to store the message in
    // in this case, it's just a short
    std::byte *buf = new std::byte[message_size()];
    // use pos to keep track of where we need to write next in the buffer
    unsigned int pos = 0;
    // copy 2 bytes (size of short) from number to buffer
    std::memcpy(buf + pos, &number, sizeof(short));
    // move pos forward 2 bytes, the amount we just copied
    // there's no point here since there's nothing left to copy, it just serves
    // as an example
    pos += sizeof(short);
    return buf;
}

