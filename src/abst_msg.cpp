#include "abst_message.hpp"


const char* MessageParseException::what() const noexcept {
    return "error parsing message from byte sequence";
}

