#include "lookup_rfc_res.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


LookupRFCResponse::LookupRFCResponse(void) : code(0), length(0), *host(""), *port(0) {}


LookupRFCResponse::LookupRFCResponse(int code, int length, std::string* host, int* port) : code(code), length(length), host(host), port(port) {}


int LookupRFCResponse::get_code() {
    return code;
}


void LookupRFCResponse::set_code(int code) {
    this->code = code;
}

int LookupRFCResponse::get_length() {
    return length;
}


void LookupRFCResponse::set_length(int length) {
    this->length = length;
}


std::string* LookupRFCResponse::get_host() {
    return host;
}


void LookupRFCResponse::set_host(std::string* host) {
    this->host = host;
}


int* LookupRFCResponse::get_port() {
    return port;
}


void LookupRFCResponse::set_length(int* port) {
    this->port = port;
}


bool LookupRFCResponse::is_valid() {
    // check that all fields are either positive or not empty
	if ( code.length() > 0 ) {
		if ( code != 200 && code != 400 && code != 404 && code != 505 ) {
			return false;
		} else if ( code == 200 ) {
			if ( length <= 0 ) {
				return false;
			}
			for ( int i = 0; i < length; i++ ) {
				if ( *(host + i).length() <= 0 || *(port + i) <= 0 ) {
					return false
				}
			}
		}
		return true;
	}
    return false;
}


unsigned int LookupRFCResponse::message_size() {
	unsigned int size = sizeof(int) + sizeof(int) + ( length * sizeof(int) ) + ( length * sizeof(int) );
	for ( int i = 0; i < length; i++ ) {
		size += *(host + i).length();
	}
    return size;
}


void LookupRFCResponse::from_bytes(std::byte *bytes) {
    // use pos to keep track of where we are in the byte sequence
    unsigned int pos = 0;
	// use len_host to keep track of host's length
	unsigned int len_host = 0;
    // copy int bytes from byte sequence to length to find code
    std::memcpy(&code, bytes + pos, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
	if ( code == 200 ) {
        // copy int bytes from byte sequence to length
        std::memcpy(&length, bytes + pos, sizeof(int));
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
		for ( int i = 0; i < length; i++ ) {
            // copy int bytes from byte sequence to length to find length of host
            std::memcpy(&len_host, bytes + pos, sizeof(int));
            // move pos forward int bytes, the amount we just copied
            pos += sizeof(int);
            // copy enough bytes from byte sequence to host to find host string
            std::memcpy((host + i), bytes + pos, len_host);
            // move pos forward length bytes, the amount we just copied
            pos += len_host;
	    	// copy int bytes from byte sequence to port
	    	std::memcpy((port + i), bytes + pos, sizeof(int));
		    // move pos forward int bytes, the amount we just copied
		    pos += sizeof(int);
		}
	}
}


std::byte* LookupRFCResponse::to_bytes() {
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
		for ( int i = 0; i < length; i++ ) {
		  // copy int bytes to buffer
		  std::memcpy(buf + pos, *(host + i).length(), sizeof(int));
		  // move pos forward int bytes, the amount we just copied
		  pos += sizeof(int);
		  // copy host bytes to buffer
		  std::memcpy(buf + pos, (host + i), host.length()));
		  // move pos forward length bytes, the amount we just copied
		  pos += host.length();
		  // copy int bytes to buffer
		  std::memcpy(buf + pos, (port + i), sizeof(int));
		  // move pos forward int bytes, the amount we just copied
		  pos += sizeof(int);
		}
	}
    return buf;
}

