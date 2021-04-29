#include "encrypted_client_socket.hpp"

#include "crypto_common.hpp"


void EncryptedClientSocket::crypto_handshake() {
    auto send = [=](void *data, size_t len) {
        return this->ClientSocket::send(data, len);
    };
    auto recv = [=](void *buf, size_t len) {
        return this->ClientSocket::recv(buf, len);
    };
    Crypto::handshake(send, recv, iv, false, enc, dec);
}


EncryptedClientSocket::EncryptedClientSocket(std::string addr, uint16_t port) :
    ClientSocket(addr, port), iv(Crypto::IV_LEN) {}


void EncryptedClientSocket::connect() {

    ClientSocket::connect();
    if (is_connected())
        crypto_handshake();

}


bool EncryptedClientSocket::send(void *data, size_t len) {

    CryptoPP::SecByteBlock ct(len);
    CryptoPP::SecByteBlock mac(Crypto::MAC_LEN);
    enc.EncryptAndAuthenticate(ct, mac, Crypto::MAC_LEN,
                iv, Crypto::IV_LEN, Crypto::CIP_HEADER, Crypto::CIP_HEADER_LEN,
                reinterpret_cast<CryptoPP::byte*>(data), len);
    ClientSocket::send(ct, len);
    return ClientSocket::send(mac, Crypto::MAC_LEN);

}


bool EncryptedClientSocket::recv(void *data, size_t len) {

    CryptoPP::SecByteBlock ct(len);
    CryptoPP::SecByteBlock mac(len);
    if (!ClientSocket::recv(ct, len))
        return false;
    if (!ClientSocket::recv(mac, Crypto::MAC_LEN))
        return false;
    return dec.DecryptAndVerify(reinterpret_cast<CryptoPP::byte*>(data), mac, Crypto::MAC_LEN,
            iv, Crypto::IV_LEN, Crypto::CIP_HEADER, Crypto::CIP_HEADER_LEN, ct, len);

}

