#include "lookup_rfc_msg.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


LookupRFCMessage::LookupRFCMessage(void) : number(0), host(""), port(0) {}


LookupRFCMessage::LookupRFCMessage(int number, std::string host, int port) : number(number), host(host), port(port) {}


int LookupRFCMessage::get_number() {
    return number;
}


void LookupRFCMessage::set_number(int number) {
    this->number = number;
}


std::string LookupRFCMessage::get_host() {
    return host;
}


void LookupRFCMessage::set_host(std::string host) {
    this->host = host;
}

int LookupRFCMessage::get_port() {
    return port;
}


void LookupRFCMessage::set_port(int port) {
    this->port = port;
}


bool LookupRFCMessage::is_valid() {
    // check that all fields are either positive or not empty
    if ( number <= 0 || host.length() <= 0 || port <= 0 ) {
        return false;
    }
    return true;
}


unsigned int LookupRFCMessage::message_size() {
    return sizeof(int) + sizeof(int) + host.length() + sizeof(int);
}


void LookupRFCMessage::from_bytes(std::byte *bytes) {
    // use pos to keep track of where we are in the byte sequence
    unsigned int pos = 0;
    // use len_host to keep track of host's length
    unsigned int len_host = 0;
    // copy number
    std::memcpy(&number, bytes + pos, sizeof(int));
    pos += sizeof(int);
    // copy int bytes from byte sequence to length to find length of host
    std::memcpy(&len_host, bytes + pos, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy enough bytes from byte sequence to host to find host string
    char host[len_host + 1];
    std::memcpy(host, bytes + pos, len_host);
    host[len_host] = '\0';
    this->host = std::string(host);
    // move pos forward length bytes, the amount we just copied
    pos += len_host;
    // copy port
    std::memcpy(&port, bytes + pos, sizeof(int));
}


std::byte* LookupRFCMessage::to_bytes() {
    // allocate a new buffer to store the message in
    std::byte *buf = new std::byte[message_size()];
    // use pos to keep track of where we need to write next in the buffer
    unsigned int pos = 0;
    // copy number
    std::memcpy(buf + pos, &number, sizeof(int));
    pos += sizeof(int);
    // copy int bytes from length to buffer
    int tmp = host.length();
    std::memcpy(buf + pos, &tmp, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy length bytes from host to buffer
    std::memcpy(buf + pos, host.c_str(), host.length());
    // move pos forward length bytes, the amount we just copied
    pos += host.length();
    // copy 2 bytes (size of int) from port to buffer
    std::memcpy(buf + pos, &port, sizeof(int));
    return buf;
}

