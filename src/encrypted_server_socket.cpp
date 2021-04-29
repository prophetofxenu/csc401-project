// ECDH key agreement
// https://cryptopp.com/wiki/Elliptic_Curve_Diffie-Hellman
// HKDF
// https://www.cryptopp.com/wiki/HKDF
// ChaCha20Poly1305
// https://www.cryptopp.com/wiki/ChaCha20Poly1305

#include "encrypted_server_socket.hpp"

#include "crypto_common.hpp"

#include <cryptopp/osrng.h>
#include <cryptopp/aes.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/secblock.h>
#include <cryptopp/oids.h>
#include <cryptopp/asn.h>
#include <cryptopp/integer.h>
#include <cryptopp/hex.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/sha.h>
#include <cryptopp/chachapoly.h>


void EncryptedServerSocket::crypto_handshake() {
    auto send = [=](void *data, size_t len) {
        return this->ServerSocket::send(data, len);
    };
    auto recv = [=](void *buf, size_t len) {
        return this->ServerSocket::recv(buf, len);
    };
    Crypto::handshake(send, recv, iv, true, enc, dec);
    crypto_init_in_progress = false;
}


EncryptedServerSocket::EncryptedServerSocket(int sock_fd) : ServerSocket(sock_fd), 
    iv(Crypto::IV_LEN) {

    crypto_init_in_progress = true;
    crypto_handshake();

}


bool EncryptedServerSocket::send(void *data, size_t len) {
    // TODO
    return false;
}


bool EncryptedServerSocket::recv(void *data, size_t len) {
    // TODO
    return false;
}

