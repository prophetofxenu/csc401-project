#include "list_rfc_res.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


ListRFCResponse::ListRFCResponse(void) : code(0) {}


ListRFCResponse::ListRFCResponse(int code, std::list<RFCHolder> holders) : code(code), holders(holders) {}


int ListRFCResponse::get_code() {
    return code;
}


void ListRFCResponse::set_code(int code) {
    this->code = code;
}


std::list<RFCHolder> ListRFCResponse::get_holders() {
    return holders;
}


void ListRFCResponse::set_holders(std::list<RFCHolder> holders) {
    this->holders = holders;
}


bool ListRFCResponse::is_valid() {
    // check that all fields are either positive or not empty
    if ( code > 0 ) {
        if ( code != 200 && code != 400 && code != 404 && code != 505 ) {
            return false;
        } else if ( code == 200 ) {
            for (auto it : holders) {
                if (it.peer.host.length() == 0 || it.peer.port <= 0)
                    return false;
            }
        }
        return true;
    }
    return false;
}


unsigned int ListRFCResponse::message_size() {
    unsigned int size = sizeof(int) + sizeof(int);
    for (auto it : holders)
        size += sizeof(int) + sizeof(int) + it.peer.host.length() + sizeof(int);
    return size;
}


void ListRFCResponse::from_bytes(std::byte *bytes) {
    // use pos to keep track of where we are in the byte sequence
    unsigned int pos = 0;
    // use len_host to keep track of list's length
    int len_list = 0;
    // use len_host to keep track of host's length
    int len_host = 0;
    // use var_host to keep track of host
    std::string var_host = "";
    // use var_host to keep track of port
    int var_port = 0;
    // rfc number
    int rfc_num;
    // copy int bytes from byte sequence to length to find code
    std::memcpy(&code, bytes + pos, sizeof(int));
    if ( code == 200 ) {
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
        // copy int bytes from byte sequence to length
        std::memcpy(&len_list, bytes + pos, sizeof(int));
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
        for ( int i = 0; i < len_list; i++ ) {
            // copy rfc num
            std::memcpy(&rfc_num, bytes + pos, sizeof(int));
            pos += sizeof(int);
            // copy int bytes from byte sequence to length to find length of host
            std::memcpy(&len_host, bytes + pos, sizeof(int));
            // move pos forward int bytes, the amount we just copied
            pos += sizeof(int);
            // copy enough bytes from byte sequence to host to find host string
            char *host = new char[len_host + 1];
            std::memcpy(host, bytes + pos, len_host);
            host[len_host] = '\0';
            pos += len_host;
            // copy port
            std::memcpy(&var_port, bytes + pos, sizeof(int));
            pos += sizeof(int);
            // add to list
            RFCHolder h { rfc_num, Client { std::string(host), var_port }};
            delete[] host;
            holders.push_back(h);
        }
    }
}


std::byte* ListRFCResponse::to_bytes() {
    // allocate a new buffer to store the message in
    std::byte *buf = new std::byte[message_size()];
    // use pos to keep track of where we need to write next in the buffer
    unsigned int pos = 0;
    // copy int bytes to buffer
    std::memcpy(buf + pos, &code, sizeof(int));
    if ( code == 200 ) {
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
        // copy int bytes to buffer
        int tmp = holders.size();
        std::memcpy(buf + pos, &tmp, sizeof(int));
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
        for (auto it : holders) {
            // copy rfc num to buffer
            tmp = it.rfc;
            std::memcpy(buf + pos, &tmp, sizeof(int));
            pos += sizeof(int);
            // copy int bytes to buffer
            tmp = it.peer.host.length();
            std::memcpy(buf + pos, &tmp, sizeof(int));
            // move pos forward int bytes, the amount we just copied
            pos += sizeof(int);
            // copy host bytes to buffer
            std::memcpy(buf + pos, it.peer.host.c_str(), it.peer.host.length());
            // move pos forward length bytes, the amount we just copied
            pos += it.peer.host.length();
            // copy int bytes to buffer
            std::memcpy(buf + pos, &(it.peer.port), sizeof(int));
            pos += sizeof(int);
        }
    }
    return buf;
}

