#include "get_rfc_res.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


GetRFCResponse::GetRFCResponse(void) : code(0), content("") {}


GetRFCResponse::GetRFCResponse(int code, std::string content) : code(code), content(content) {}


int GetRFCResponse::get_code() {
    return code;
}


void GetRFCResponse::set_code(int code) {
    this->code = code;
}


std::string GetRFCResponse::get_content() {
    return content;
}


void GetRFCResponse::set_content(std::string content) {
    this->content = content;
}


bool GetRFCResponse::is_valid() {
    // check that all fields are either positive or not empty
    if ( code > 0 ) {
        if ( code != 200 && code != 400 && code != 404 && code != 505 ) {
            return false;
        } else if ( code == 200 ) {
            if ( content.length() <= 0 ) {
                return false;
            }
        }
        return true;
    }
    return false;
}


unsigned int GetRFCResponse::message_size() {
    return sizeof(int) + sizeof(int) + content.length();
}


void GetRFCResponse::from_bytes(std::byte *bytes) {
    // use pos to keep track of where we are in the byte sequence
    unsigned int pos = 0;
    // copy int bytes from byte sequence to length to find code
    std::memcpy(&code, bytes + pos, sizeof(int));
    if ( code == 200 ) {
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
        // copy int bytes from byte sequence to length to find length
        int length;
        std::memcpy(&length, bytes + pos, sizeof(int));
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
        // copy enough bytes from byte sequence to find string
        char *content = new char[length + 1];
        std::memcpy(content, bytes + pos, length);
        content[length] = '\0';
        this->content = std::string(content);
        delete[] content;
        // move pos forward length bytes, the amount we just copied
        pos += length;
    }
}


std::byte* GetRFCResponse::to_bytes() {
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
        int tmp = content.length();
        std::memcpy(buf + pos, &tmp, sizeof(int));
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
        // copy length bytes from to buffer
        std::memcpy(buf + pos, content.c_str(), content.length());
    }
    return buf;
}

