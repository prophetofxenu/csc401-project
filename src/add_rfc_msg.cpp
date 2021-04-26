#include "add_rfc_msg.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


AddRFCMessage::AddRFCMessage(void) : host(""), port(0), number(0) {}


AddRFCMessage::AddRFCMessage(std::string host, int port, int number) : host(host), port(port), number(number) {}


AddRFCMessage::~AddRFCMessage() {}


std::string AddRFCMessage::get_host() {
    return host;
}


void AddRFCMessage::set_host(std::string host) {
    this->host = host;
}

int AddRFCMessage::get_port() {
    return port;
}


void AddRFCMessage::set_port(int port) {
    this->port = port;
}


int AddRFCMessage::get_number() {
    return number;
}


void AddRFCMessage::set_number(int number) {
    this->number = number;
}


bool AddRFCMessage::is_valid() {
    // check that all fields are either positive or not empty
    if ( host.length() <= 0 || port <= 0 || number <= 0 ) {
        return false;
    }
    return true;
}


unsigned int AddRFCMessage::message_size() {
    return sizeof(int) + host.length() + sizeof(int) + sizeof(int);
}


void AddRFCMessage::from_bytes(std::byte *bytes) {
    // use pos to keep track of where we are in the byte sequence
    unsigned int pos = 0;
    // use len_host to keep track of host's length
    unsigned int len_host = 0;
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
    // copy 2 bytes (size of short) from byte sequence to port
    std::memcpy(&port, bytes + pos, sizeof(int));
    // move pos forward 2 bytes, the amount we just copied
    pos += sizeof(int);
    // copy enough bytes from byte sequence to title to find title string
    std::memcpy(&number, bytes + pos, sizeof(int));
}


std::byte* AddRFCMessage::to_bytes() {
    // allocate a new buffer to store the message in
    std::byte *buf = new std::byte[message_size()];
    // use pos to keep track of where we need to write next in the buffer
    unsigned int pos = 0;
    int tmp = host.length();
    // copy int bytes from length to buffer
    std::memcpy(buf + pos, &tmp, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy length bytes from host to buffer
    std::memcpy(buf + pos, host.c_str(), host.length());
    // move pos forward length bytes, the amount we just copied
    pos += host.length();
    // copy 2 bytes (size of int) from port to buffer
    std::memcpy(buf + pos, &port, sizeof(int));
    // move pos forward 2 bytes, the amount we just copied
    pos += sizeof(int);
    // copy int bytes from length to buffer
    std::memcpy(buf + pos, &number, sizeof(int));
    return buf;
}

