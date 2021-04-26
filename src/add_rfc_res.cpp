#include "add_rfc_res.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


AddRFCResponse::AddRFCResponse(void) : code(0), host(""), port(0) {}


AddRFCResponse::AddRFCResponse(int code, std::string host, int port) : code(code), host(host), port(port) {}


int AddRFCResponse::get_code() {
    return code;
}


void AddRFCResponse::set_code(int code) {
    this->code = code;
}


std::string AddRFCResponse::get_host() {
    return host;
}


void AddRFCResponse::set_host(std::string host) {
    this->host = host;
}


int AddRFCResponse::get_port() {
    return port;
}


bool AddRFCResponse::is_valid() {
    // check that all fields are either positive or not empty
    if ( code > 0 ) {
        if ( code != 200 && code != 400 && code != 404 && code != 505 ) {
            return false;
        } else if ( code == 200 ) {
            if ( port <= 0 || host.length() <= 0 ) {
                return false;
            }
        }
        return true;
    }
    return false;
}


unsigned int AddRFCResponse::message_size() {
    if (code == 200)
        return sizeof(int) + sizeof(int) + sizeof(int) + host.length();
    else
        return sizeof(int);
}


void AddRFCResponse::from_bytes(std::byte *bytes) {
    // use pos to keep track of where we are in the byte sequence
    unsigned int pos = 0;
    // copy int bytes from byte sequence to length to find code
    std::memcpy(&code, bytes + pos, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    if ( code == 200 ) {
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
        // copy int bytes from byte sequence to port
        std::memcpy(&port, bytes + pos, sizeof(int));
    }
}


std::byte* AddRFCResponse::to_bytes() {
    // allocate a new buffer to store the message in
    std::byte *buf = new std::byte[message_size()];
    // use pos to keep track of where we need to write next in the buffer
    unsigned int pos = 0;
    // copy int bytes to buffer
    std::memcpy(buf + pos, &code, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    if ( code == 200 ) {
        pos += sizeof(int);
        // copy int bytes to buffer
        int tmp = host.length();
        std::memcpy(buf + pos, &tmp, sizeof(int));
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
        // copy host bytes to buffer
        std::memcpy(buf + pos, host.c_str(), host.length());
        // move pos forward length bytes, the amount we just copied
        pos += host.length();
        // copy int bytes to buffer
        std::memcpy(buf + pos, &port, sizeof(int));
    }
    return buf;
}

