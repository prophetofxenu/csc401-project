#include "list_rfc_res.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


ListRFCResponse::ListRFCResponse(void) : code(0), host(nullptr), port(nullptr) {}


ListRFCResponse::ListRFCResponse(int code, std::list<std::string> host, std::list<int> port) : code(code), host(host), port(port) {}


int ListRFCResponse::get_code() {
    return code;
}


void ListRFCResponse::set_code(int code) {
    this->code = code;
}


std::list<std::string> ListRFCResponse::get_host() {
    return host;
}


void ListRFCResponse::set_host(std::list<std::string> host) {
    this->host = host;
}


std::list<std::int> ListRFCResponse::get_port() {
    return port;
}


void ListRFCResponse::set_length(std::list<std::int> port) {
    this->port = port;
}


bool ListRFCResponse::is_valid() {
    // check that all fields are either positive or not empty
	if ( code > 0 ) {
		if ( code != 200 && code != 400 && code != 404 && code != 505 ) {
			return false;
		} else if ( code == 200 ) {
			for ( int i = 0; i < host.size(); i++ ) {
				if ( host[i].length() <= 0 || port[i] <= 0 ) {
					return false
				}
			}
		}
		return true;
	}
    return false;
}


unsigned int ListRFCResponse::message_size() {
	unsigned int size = sizeof(int) + ( host.size() * sizeof(int) ) + ( port.size() * sizeof(int) );
	for ( int i = 0; i < length; i++ ) {
		size += host[i].length();
	}
    return size;
}


void ListRFCResponse::from_bytes(std::byte *bytes) {
    // use pos to keep track of where we are in the byte sequence
    unsigned int pos = 0;
	// use len_host to keep track of list's length
	unsigned int len_list = 0;
	// use len_host to keep track of host's length
	unsigned int len_host = 0;
	// use var_host to keep track of host
	std::string var_host = "";
	// use var_host to keep track of port
	int var_port = 0;
    // copy int bytes from byte sequence to length to find code
    std::memcpy(&code, bytes + pos, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
	if ( code == 200 ) {
        // copy int bytes from byte sequence to length
        std::memcpy(&len_list, bytes + pos, sizeof(int));
        // move pos forward int bytes, the amount we just copied
        pos += sizeof(int);
		// initialize host and port
		host = new std::list[len_list];
		port = new std::list[len_list];
		for ( int i = 0; i < length; i++ ) {
            // copy int bytes from byte sequence to length to find length of host
            std::memcpy(&len_host, bytes + pos, sizeof(int));
            // move pos forward int bytes, the amount we just copied
            pos += sizeof(int);
            // copy enough bytes from byte sequence to host to find host string
            std::memcpy(&var_host, bytes + pos, len_host);
			host.push_back(var_host);
            // move pos forward length bytes, the amount we just copied
            pos += len_host;
	    	// copy int bytes from byte sequence to port
	    	std::memcpy(&var_port, bytes + pos, sizeof(int));
			port.push_back(var_port);
		    // move pos forward int bytes, the amount we just copied
		    pos += sizeof(int);
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
	// move pos forward int bytes, the amount we just copied
	pos += sizeof(int);
	if ( code == 200 ) {
        // copy int bytes to buffer
        std::memcpy(buf + pos, &(host.size()), sizeof(int));
	    // move pos forward int bytes, the amount we just copied
	    pos += sizeof(int);
		for ( int i = 0; i < length; i++ ) {
		  // copy int bytes to buffer
		  std::memcpy(buf + pos, host[i].length(), sizeof(int));
		  // move pos forward int bytes, the amount we just copied
		  pos += sizeof(int);
		  // copy host bytes to buffer
		  std::memcpy(buf + pos, &(host[i]), host[i].length()));
		  // move pos forward length bytes, the amount we just copied
		  pos += host[i].length();
		  // copy int bytes to buffer
		  std::memcpy(buf + pos, &(port[i]), sizeof(int));
		  // move pos forward int bytes, the amount we just copied
		  pos += sizeof(int);
		}
	}
    return buf;
}

