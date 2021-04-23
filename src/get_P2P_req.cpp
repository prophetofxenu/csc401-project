#include "get_P2P_req.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


GetP2PRequest::GetP2PRequest(void) : host(""), os("") {}


GetP2PRequest::GetP2PRequest(std::string host, std::string os) : host(host), os(os) {}


std::string GetP2PRequest::get_host() {
    return host;
}


void GetP2PRequest::set_host(std::string host) {
    this->host = host;
}


std::string GetP2PRequest::get_os() {
    return os;
}


void GetP2PRequest::set_os(std::string os) {
    this->os = os;
}


bool GetP2PRequest::is_valid() {
    // check that all fields are either positive or not empty
	if ( host.length() <= 0 || os.length() <= 0 ) {
		return false;
	}
    return true;
}


unsigned int GetP2PRequest::message_size() {
    return host.length() + os.length();
}


void GetRFCMessage::from_bytes(std::byte *bytes) {
    // use pos to keep track of where we are in the byte sequence
    unsigned int pos = 0;
	// use len_host to keep track of host's length
	unsigned int len_host = 0;
	// use len_host to keep track of host's length
	unsigned int len_os = 0;
    // copy int bytes from byte sequence to length to find length of host
    std::memcpy(&len_host, bytes + pos, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy enough bytes from byte sequence to host to find host string
    std::memcpy(&host, bytes + pos, len_host);
    // move pos forward length bytes, the amount we just copied
    pos += len_host;
    // copy int bytes from byte sequence to length to find length of os
    std::memcpy(&len_os, bytes + pos, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy enough bytes from byte sequence to host to find host string
    std::memcpy(&host, bytes + pos, len_os);
    // move pos forward length bytes, the amount we just copied
    pos += len_os;
}


std::byte* GetRFCMessage::to_bytes() {
    // allocate a new buffer to store the message in
    // in this case, it's just a short
    std::byte *buf = new std::byte[message_size()];
    // use pos to keep track of where we need to write next in the buffer
    unsigned int pos = 0;
    // copy int bytes from length to buffer
    std::memcpy(buf + pos, &host.length(), sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy length bytes from host to buffer
    std::memcpy(buf + pos, &host, host.length());
    // move pos forward length bytes, the amount we just copied
    pos += host.length();
    // copy int bytes from length to buffer
    std::memcpy(buf + pos, &os.length(), sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy length bytes from os to buffer
    std::memcpy(buf + pos, &os, os.length());
    // move pos forward length bytes, the amount we just copied
    pos += os.length();
    return buf;
}

