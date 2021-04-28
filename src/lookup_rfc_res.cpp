#include "lookup_rfc_res.hpp"

#include "common.hpp"

#include <cstddef>
using std::byte;
#include <cstring>
#include <arpa/inet.h>


LookupRFCResponse::LookupRFCResponse(void) : code(0) {}


LookupRFCResponse::LookupRFCResponse(int code, std::list<Client> hosts) : code(code), hosts(hosts) {}


int LookupRFCResponse::get_code() {
    return code;
}


void LookupRFCResponse::set_code(int code) {
    this->code = code;
}


std::list<Client> LookupRFCResponse::get_hosts() {
    return hosts;
}


void LookupRFCResponse::set_hosts(std::list<Client> hosts) {
    this->hosts = hosts;
}


bool LookupRFCResponse::is_valid() {
    // check that all fields are either positive or not empty
    if ( code > 0 ) {
        if ( code != 200 && code != 400 && code != 404 && code != 505 ) {
            return false;
        } else if ( code == 200 ) {
            for (auto it : hosts) {
                if (it.host.length() == 0 || it.port <= 0)
                    return false;
            }
        }
        return true;
    }
    return false;
}


unsigned int LookupRFCResponse::message_size() {
    unsigned int size = sizeof(int) + sizeof(int);
    for (auto it : hosts) {
        size += sizeof(int) + it.host.length() + sizeof(int);
    }
    return size;
}


void LookupRFCResponse::from_bytes(std::byte *bytes) {
    hosts.clear();
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
    // copy int bytes from byte sequence to length to find code
    std::memcpy(&code, bytes + pos, sizeof(int));
    code = ntohl(code);
    if ( code == 200 ) {
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
        // copy int bytes from byte sequence to length
        std::memcpy(&len_list, bytes + pos, sizeof(int));
        len_list = ntohl(len_list);
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
        for ( int i = 0; i < len_list; i++ ) {
            // copy int bytes from byte sequence to length to find length of host
            std::memcpy(&len_host, bytes + pos, sizeof(int));
            len_host = ntohl(len_host);
            // move pos forward int bytes, the amount we just copied
            pos += sizeof(int);
            // copy host
            char host[len_host + 1];
            std::memcpy(host, bytes + pos, len_host);
            host[len_host] = '\0';
            pos += len_host;
            // copy port
            std::memcpy(&var_port, bytes + pos, sizeof(int));
            var_port = ntohl(var_port);
            pos += sizeof(int);
            // add to list
            Client c { std::string(host), var_port };
            hosts.push_back(c);
        }
    }
}


std::byte* LookupRFCResponse::to_bytes() {
    if (!is_valid())
        return nullptr;
    // allocate a new buffer to store the message in
    std::byte *buf = new std::byte[message_size()];
    // use pos to keep track of where we need to write next in the buffer
    unsigned int pos = 0;
    // copy int bytes to buffer
    int tmp = htonl(code);
    std::memcpy(buf + pos, &tmp, sizeof(int));
    if ( code == 200 ) {
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
        // copy int bytes to buffer
        tmp = hosts.size();
        tmp = htonl(tmp);
        std::memcpy(buf + pos, &tmp, sizeof(int));
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
        for (auto it : hosts) {
            // copy host length
            tmp = it.host.length();
            tmp = htonl(tmp);
            std::memcpy(buf + pos, &tmp, sizeof(int));
            pos += sizeof(int);
            // copy host bytes to buffer
            std::memcpy(buf + pos, it.host.c_str(), it.host.length());
            // move pos forward length bytes, the amount we just copied
            pos += it.host.length();
            // copy int bytes to buffer
            tmp = it.port;
            tmp = htonl(tmp);
            std::memcpy(buf + pos, &tmp, sizeof(int));
            // move pos forward int bytes, the amount we just copied
            pos += sizeof(int);
        }
    }
    return buf;
}

