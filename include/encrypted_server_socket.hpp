#ifndef ENCRYPTED_SERVER_SOCK_H
#define ENCRYPTED_SERVER_SOCK_H

#include "server_socket.hpp"
#include "crypto_common.hpp"

#include <crypto++/secblock.h>
#include <crypto++/chachapoly.h>


class EncryptedServerSocket : public ServerSocket {

private:

    CryptoPP::SecByteBlock iv;
    CryptoPP::ChaCha20Poly1305::Encryption enc;
    CryptoPP::ChaCha20Poly1305::Decryption dec;

    void crypto_handshake();

public:

    EncryptedServerSocket(int sock_fd);
    virtual bool send(void *data, size_t len);
    virtual bool recv(void *buf, size_t len);

};


#endif

