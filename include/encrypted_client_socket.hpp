#ifndef ENCRYPTED_CLIENT_SOCK_H
#define ENCRYPTED_CLIENT_SOCK_H


#include "client_socket.hpp"
#include "crypto_common.hpp"

#include <cryptopp/secblock.h>
#include <cryptopp/chachapoly.h>

#include <string>


class EncryptedClientSocket : public ClientSocket {

private:

    bool crypto_init_in_progress;

    CryptoPP::SecByteBlock iv;
    CryptoPP::ChaCha20Poly1305::Encryption enc;
    CryptoPP::ChaCha20Poly1305::Decryption dec;

    void crypto_handshake();

public:

    EncryptedClientSocket(std::string addr, uint16_t port);
    virtual void connect();
    virtual bool send(void *data, size_t len);
    virtual bool recv(void *buf, size_t len);

};


#endif

