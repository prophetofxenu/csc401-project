#include "get_rfc_res.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


GetRFCResponse::GetRFCResponse(void) : code(0), length(0), content("") {}


GetRFCResponse::GetRFCResponse(int code, int length, std::string content) : code(code), length(length), content(content) {}


int GetRFCResponse::get_code() {
    return code;
}


void GetRFCResponse::set_code(int code) {
    this->code = code;
}


int GetRFCResponse::get_length() {
    return length;
}


void GetRFCResponse::set_length(int length) {
    this->length = length;
}


std::string GetRFCResponse::get_content() {
    return content;
}


void GetRFCResponse::set_content(std::string content) {
    this->content = content;
}


bool GetRFCResponse::is_valid() {
    // check that all fields are either positive or not empty
	if ( code.length() > 0 ) {
		if ( code != 200 && code != 400 && code != 404 && code != 505 ) {
			return false;
		} else if ( code == 200 ) {
			if ( length <= 0 || content.length <= 0 ) {
				return false;
			}
		}
		return true
	}
    return false;
}


unsigned int GetRFCResponse::message_size() {
    return sizeof(int) + sizeof(int) + sizeof(int) + content.length();
}


void GetRFCResponse::from_bytes(std::byte *bytes) {
    // use pos to keep track of where we are in the byte sequence
    unsigned int pos = 0;
    // copy int bytes from byte sequence to length to find code
    std::memcpy(&code, bytes + pos, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
	if ( code == 200 ) {
		// copy int bytes from byte sequence to length to find length
		std::memcpy(&length, bytes + pos, sizeof(int));
		// move pos forward int bytes, the amount we just copied
		pos += sizeof(int);
		// copy enough bytes from byte sequence to find string
		std::memcpy(&content, bytes + pos, length);
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
	// move pos forward int bytes, the amount we just copied
	pos += sizeof(int);
	if ( code == 200 ) {
		// copy int bytes to buffer
		std::memcpy(buf + pos, &length, sizeof(int));
		// move pos forward int bytes, the amount we just copied
		pos += sizeof(int);
		// copy int bytes to buffer
		std::memcpy(buf + pos, content.length(), sizeof(int));
		// move pos forward int bytes, the amount we just copied
		pos += sizeof(int);
		// copy length bytes from to buffer
		std::memcpy(buf + pos, &content, length));
		// move pos forward length bytes, the amount we just copied
		pos += length;
	}
    return buf;
}

