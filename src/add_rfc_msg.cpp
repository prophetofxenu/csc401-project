#include "add_rfc_msg.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


AddRFCMessage::AddRFCMessage(void) : number(0) {}


AddRFCMessage::AddRFCMessage(int number) : number(number) {}


int AddRFCMessage::get_number() {
    return number;
}


void AddRFCMessage::set_number(int number) {
    this->number = number;
}


bool AddRFCMessage::is_valid() {
    return number > 0;
}


unsigned int AddRFCMessage::message_size() {
    return sizeof(int);
}


void AddRFCMessage::from_bytes(std::byte *bytes) {
    // copy enough bytes from byte sequence to title to find title string
    std::memcpy(&number, bytes, sizeof(int));
}


std::byte* AddRFCMessage::to_bytes() {
    if (!is_valid())
        return nullptr;
    // allocate a new buffer to store the message in
    std::byte *buf = new std::byte[message_size()];
    // copy int bytes from length to buffer
    std::memcpy(buf, &number, sizeof(int));
    return buf;
}

