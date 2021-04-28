#ifndef RFC_COMMON_H
#define RFC_COMMON_H

#define PROTOCOL_VERSION 1

#include <string>


struct Client {
    std::string host;
    int port;
};


/* Represents an association between a client and an RFC.
 * As in, a client has a local copy of an RFC. */
struct RFCHolder {
    int rfc;
    Client peer;
};


enum class P2SMessage {
    HELLO,
    ADD,
    LIST,
    LOOKUP,
    DISCONNECT
};


enum class P2PMessage {
    HELLO,
    GET,
    DISCONNECT
};


#endif

