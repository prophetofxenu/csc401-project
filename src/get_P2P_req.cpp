#include "get_rfc_msg.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


GetP2PRequest::GetRFCMessage(void) : lengthOs(0), lengthHost(0), host(""), os("") {}


GetP2PRequest::GetRFCMessage(int lengthHost, int lengthOs, std::string host, std::string os) : lengthOs(lengthOs), lengthHost(lengthHost), host(host), os(os) {}


int GetP2PRequest::get_lengthHost() {
    return lengthHost;
}


void GetP2PRequest::set_lengthHost(int length) {
    this->lengthHost = length;
}


int GetP2PRequest::get_lengthOs() {
    return lengthOs;
}


void GetP2PRequest::set_lengthHost(int length) {
    this->lengthOs = length;
}


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
	if ( lengthHost <= 0 || lengthOs <= 0 || host.length() <= 0 || os.length() <= 0 ) {
		return false;
	}
    return true;
}


unsigned int GetRFCMessage::message_size() {
    return sizeof(short) + sizeof(host) + sizeof(os);
}


void GetRFCMessage::from_bytes(std::byte *bytes) {
    // use pos to keep track of where we are in the byte sequence
    unsigned int pos = 0;
    // copy int bytes from byte sequence to length to find length of host
    std::memcpy(&lengthHost, bytes + pos, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy enough bytes from byte sequence to host to find host string
    std::memcpy(&host, bytes + pos, lengthHost);
    // move pos forward length bytes, the amount we just copied
    pos += lengthHost;
    // copy int bytes from byte sequence to length to find length of os
    std::memcpy(&lengthOs, bytes + pos, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy enough bytes from byte sequence to host to find host string
    std::memcpy(&host, bytes + pos, lengthOs);
    // move pos forward length bytes, the amount we just copied
    pos += lengthOs;
}


std::byte* GetRFCMessage::to_bytes() {
    // allocate a new buffer to store the message in
    // in this case, it's just a short
    std::byte *buf = new std::byte[message_size()];
    // use pos to keep track of where we need to write next in the buffer
    unsigned int pos = 0;
    // copy int bytes from length to buffer
    std::memcpy(buf + pos, &lengthHost, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy length bytes from host to buffer
    std::memcpy(buf + pos, &host, lengthHost);
    // move pos forward length bytes, the amount we just copied
    pos += lengthHost;
    // copy int bytes from length to buffer
    std::memcpy(buf + pos, &lengthOs, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy length bytes from os to buffer
    std::memcpy(buf + pos, &os, lengthOs);
    // move pos forward length bytes, the amount we just copied
    pos += lengthOs;
    return buf;
}

