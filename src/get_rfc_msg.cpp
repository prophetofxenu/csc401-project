#include "get_rfc_msg.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


GetRFCMessage::GetRFCMessage(void) : number(0), host(""), os("") {}


GetRFCMessage::GetRFCMessage(short number, std::string host, std::string os) : number(number), host(host), os(os) {}


short GetRFCMessage::get_number() {
    return number;
}


void GetRFCMessage::set_number(short number) {
    this->number = number;
}

std::string GetRFCMessage::get_host() {
    return host;
}


void GetRFCMessage::set_host(std::string host) {
    this->host = host;
}


std::string GetRFCMessage::get_os() {
    return os;
}


void GetRFCMessage::set_os(std::string os) {
    this->os = os;
}


bool GetRFCMessage::is_valid() {
    // check that all fields are either positive or not empty
	if ( host.length() <= 0 || os.length() <= 0 ) {
		return false;
	}
    return true;
}


unsigned int GetRFCMessage::message_size() {
    return sizeof(short) + host.length() + os.length();
}


void GetRFCMessage::from_bytes(std::byte *bytes) {
    // use pos to keep track of where we are in the byte sequence
    unsigned int pos = 0;
	// use len_host to keep track of host's length
	unsigned int len_host = 0;
	// use len_host to keep track of host's length
	unsigned int len_os = 0;
    // copy 2 bytes (size of short) from byte sequence to number
    std::memcpy(&number, bytes + pos, sizeof(short));
    // move pos forward 2 bytes, the amount we just copied
    pos += sizeof(short);
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
    // copy 2 bytes (size of short) from number to buffer
    std::memcpy(buf + pos, &number, sizeof(short));
    // move pos forward 2 bytes, the amount we just copied
    pos += sizeof(short);
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

