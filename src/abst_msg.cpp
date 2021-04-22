#include "abst_msg.hpp"


const char* MessageParseException::what() const noexcept {
    return "error parsing message from byte sequence";
}

