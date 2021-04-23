#ifndef ABST_MSG_H
#define ABST_MSG_H

#include <exception>
#include <cstddef>


/* Represents a message to be exchanged between two hosts.
 * The message can be converted to and from a sequence of bytes to be sent over sockets. */
class AbstMessage {

    /* Returns true if all the necessary fields have been set in the message for
     * to_bytes to be called. */
    virtual bool is_valid() = 0;

    /* Returns the size of the message as returned by from_bytes. */
    virtual unsigned int message_size() = 0;

    /* Sets the fields of the message from a sequence of bytes. */
    virtual void from_bytes(std::byte *bytes) = 0;

    /* Exports the fields of the message to a sequence of bytes. */
    virtual std::byte* to_bytes() = 0;

};


/* Thrown by from_bytes when an error parsing the byte sequence occurs. */
class MessageParseException : public std::exception {
    virtual const char* what() const noexcept;
};


#endif

