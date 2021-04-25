#include "lookup_rfc_msg.hpp"

#include <cstddef>
using std::byte;
#include <cstring>


LookupRFCMessage::LookupRFCMessage(void) : host(""), port(0), title("") {}


LookupRFCMessage::LookupRFCMessage(std::string host, int port, std::string title) : host(host), port(port), title(title) {}

std::string LookupRFCMessage::get_host() {
    return host;
}


void LookupRFCMessage::set_host(std::string host) {
    this->host = host;
}

int LookupRFCMessage::get_port() {
    return port;
}


void LookupRFCMessage::set_port(int port) {
    this->port = port;
}


std::string LookupRFCMessage::get_title() {
    return title;
}


void LookupRFCMessage::set_os(std::string title) {
    this->title = title;
}


bool LookupRFCMessage::is_valid() {
    // check that all fields are either positive or not empty
	if ( host.length() <= 0 || port <= 0 || title.length() <= 0 ) {
		return false;
	}
    return true;
}


unsigned int LookupRFCMessage::message_size() {
    return sizeof(int) + host.length() + title.length();
}


void LookupRFCMessage::from_bytes(std::byte *bytes) {
    // use pos to keep track of where we are in the byte sequence
    unsigned int pos = 0;
	// use len_host to keep track of host's length
	unsigned int len_host = 0;
	// use len_host to keep track of host's length
	unsigned int len_title = 0;
    // copy int bytes from byte sequence to length to find length of host
    std::memcpy(&len_host, bytes + pos, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy enough bytes from byte sequence to host to find host string
    std::memcpy(&host, bytes + pos, len_host);
    // move pos forward length bytes, the amount we just copied
    pos += len_host;
    // copy 2 bytes (size of short) from byte sequence to port
    std::memcpy(&port, bytes + pos, sizeof(int));
    // move pos forward 2 bytes, the amount we just copied
    pos += sizeof(int);
    // copy int bytes from byte sequence to length to find length of title
    std::memcpy(&len_title, bytes + pos, sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy enough bytes from byte sequence to title to find title string
    std::memcpy(&title, bytes + pos, len_title);
    // move pos forward length bytes, the amount we just copied
    pos += len_title;
}


std::byte* LookupRFCMessage::to_bytes() {
    // allocate a new buffer to store the message in
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
    // copy 2 bytes (size of int) from port to buffer
    std::memcpy(buf + pos, &port, sizeof(int));
    // move pos forward 2 bytes, the amount we just copied
    pos += sizeof(int);
    // copy int bytes from length to buffer
    std::memcpy(buf + pos, &title.length(), sizeof(int));
    // move pos forward int bytes, the amount we just copied
    pos += sizeof(int);
    // copy length bytes from title to buffer
    std::memcpy(buf + pos, &title, title.length());
    // move pos forward length bytes, the amount we just copied
    pos += title.length();
    return buf;
}

