#include "list_rfc_msg.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


ListRFCMessage::ListRFCMessage(void) : host(""), port(0) {}


ListRFCMessage::ListRFCMessage(std::string host, int port) : host(host), port(port) {}


std::string ListRFCMessage::get_host() {
    return host;
}


void ListRFCMessage::set_host(std::string host) {
    this->host = host;
}


int ListRFCMessage::get_port() {
    return port;
}


void ListRFCMessage::set_port(int port) {
    this->port = port;
}


bool ListRFCMessage::is_valid() {
    // check that all fields are either positive or not empty
    if ( host.length() <= 0 || port <= 0 ) {
        return false;
    }
    return true;
}


unsigned int ListRFCMessage::message_size() {
    return sizeof(int) + host.length() + sizeof(int);
}


void ListRFCMessage::from_bytes(std::byte *bytes) {
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
}


std::byte* ListRFCMessage::to_bytes() {
    if (!is_valid())
        return nullptr;
    // allocate a new buffer to store the message in
    std::byte *buf = new std::byte[message_size()];
    // use pos to keep track of where we need to write next in the buffer
    unsigned int pos = 0;
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

