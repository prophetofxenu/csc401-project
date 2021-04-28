#include "lookup_rfc_msg.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


LookupRFCMessage::LookupRFCMessage(void) : number(0) {}


LookupRFCMessage::LookupRFCMessage(int number) : number(number) {}


int LookupRFCMessage::get_number() {
    return number;
}


void LookupRFCMessage::set_number(int number) {
    this->number = number;
}


bool LookupRFCMessage::is_valid() {
    // check that all fields are either positive or not empty
    return number > 0;
}


unsigned int LookupRFCMessage::message_size() {
    return sizeof(int);
}


void LookupRFCMessage::from_bytes(std::byte *bytes) {
    // copy number
    std::memcpy(&number, bytes, sizeof(int));
}


std::byte* LookupRFCMessage::to_bytes() {
    if (!is_valid())
        return nullptr;
    // allocate a new buffer to store the message in
    std::byte *buf = new std::byte[message_size()];
    // copy number
    std::memcpy(buf, &number, sizeof(int));
    return buf;
}

