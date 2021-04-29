#ifndef CRYPT_COMMON_H
#define CRYPT_COMMON_H

#include "abst_socket.hpp"

#include <cryptopp/secblock.h>
#include <cryptopp/oids.h>
#include <cryptopp/chachapoly.h>
#include <functional>


namespace Crypto {

    static constexpr CryptoPP::byte HKDF_SALT[] = "HKDFSALT";
    static constexpr int HKDF_SALT_LEN = sizeof(HKDF_SALT) - 1;
    static constexpr CryptoPP::byte HKDF_INFO[] = "HKDFINFO";
    static constexpr int HKDF_INFO_LEN = sizeof(HKDF_INFO) - 1;
    static const int KEY_LEN = 32;
    static const int IV_LEN = 12;
    static const int MAC_LEN = 16;
    static constexpr CryptoPP::byte CIP_HEADER[] = "CIPHEADER";
    static constexpr int CIP_HEADER_LEN = sizeof(CIP_HEADER) - 1;
    static constexpr char CONFIRM_PHRASE[] = "CONFIRM";
    static constexpr int CONFIRM_PHRASE_LEN = sizeof(CONFIRM_PHRASE) - 1;

    bool handshake(std::function<bool(void*,size_t)> send,
            std::function<bool(void*,size_t)> recv,
            CryptoPP::SecByteBlock iv, bool gen_iv,
            CryptoPP::ChaCha20Poly1305::Encryption &enc,
            CryptoPP::ChaCha20Poly1305::Decryption &dec);

}


#endif

