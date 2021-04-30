// ECDH key agreement
// https://cryptopp.com/wiki/Elliptic_Curve_Diffie-Hellman
// HKDF
// https://www.cryptopp.com/wiki/HKDF
// ChaCha20Poly1305
// https://www.cryptopp.com/wiki/ChaCha20Poly1305

#include "encrypted_server_socket.hpp"

#include "crypto_common.hpp"

#include <crypto++/secblock.h>

#include <iostream>


void EncryptedServerSocket::crypto_handshake() {
    auto send = [=](void *data, size_t len) {
        return this->ServerSocket::send(data, len);
    };
    auto recv = [=](void *buf, size_t len) {
        return this->ServerSocket::recv(buf, len);
    };
    Crypto::handshake(send, recv, iv, true, enc, dec);
}


EncryptedServerSocket::EncryptedServerSocket(int sock_fd) : ServerSocket(sock_fd), 
    iv(Crypto::IV_LEN) {

    crypto_handshake();
}


bool EncryptedServerSocket::send(void *data, size_t len) {

    CryptoPP::SecByteBlock ct(len);
    CryptoPP::SecByteBlock mac(Crypto::MAC_LEN);
    enc.EncryptAndAuthenticate(ct, mac, Crypto::MAC_LEN,
                iv, Crypto::IV_LEN, Crypto::CIP_HEADER, Crypto::CIP_HEADER_LEN,
                reinterpret_cast<CryptoPP::byte*>(data), len);
    ServerSocket::send(ct, len);
    return ServerSocket::send(mac, Crypto::MAC_LEN);

}


bool EncryptedServerSocket::recv(void *data, size_t len) {

    CryptoPP::SecByteBlock ct(len);
    CryptoPP::SecByteBlock mac(len);
    if (!ServerSocket::recv(ct, len))
        return false;
    if (!ServerSocket::recv(mac, Crypto::MAC_LEN))
        return false;
    return dec.DecryptAndVerify(reinterpret_cast<CryptoPP::byte*>(data), mac, Crypto::MAC_LEN,
            iv, Crypto::IV_LEN, Crypto::CIP_HEADER, Crypto::CIP_HEADER_LEN, ct, len);

}

