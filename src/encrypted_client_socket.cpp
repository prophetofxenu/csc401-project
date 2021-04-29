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
    crypto_init_in_progress = false;
}


EncryptedClientSocket::EncryptedClientSocket(std::string addr, uint16_t port) :
    ClientSocket(addr, port), crypto_init_in_progress(true), iv(Crypto::IV_LEN) {}


void EncryptedClientSocket::connect() {

    ClientSocket::connect();
    if (is_connected())
        crypto_handshake();

}


bool EncryptedClientSocket::send(void *data, size_t len) {
    if (crypto_init_in_progress)
        return ClientSocket::send(data, len);
    // TODO
    return false;
}


bool EncryptedClientSocket::recv(void *data, size_t len) {
    if (crypto_init_in_progress)
        return ClientSocket::recv(data, len);
    // TODO
    return false;
}

