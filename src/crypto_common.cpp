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

#include <iostream>


bool Crypto::handshake(std::function<bool(void* data, size_t len)> send,
        std::function<bool(void* buf, size_t len)> recv,
        CryptoPP::SecByteBlock iv, bool gen_iv,
        CryptoPP::ChaCha20Poly1305::Encryption &enc,
        CryptoPP::ChaCha20Poly1305::Decryption &dec) {


    // generate server pieces of ECDH key
    CryptoPP::OID curve = CryptoPP::ASN1::secp384r1();
    CryptoPP::AutoSeededX917RNG<CryptoPP::AES> rng;
    CryptoPP::ECDH<CryptoPP::ECP>::Domain dh(curve);
    CryptoPP::SecByteBlock serv_priv_ecdh(dh.PrivateKeyLength()),
        serv_pub_ecdh(dh.PublicKeyLength());
    dh.GenerateKeyPair(rng, serv_priv_ecdh, serv_pub_ecdh);
    // send public piece to client
    int serv_pub_ecdh_size = serv_pub_ecdh.size();
    send(&serv_pub_ecdh_size, sizeof(int));
    send(serv_pub_ecdh.data(), serv_pub_ecdh_size);

    // get client public piece
    int client_pub_ecdh_size;
    recv(&client_pub_ecdh_size, sizeof(int));
    CryptoPP::SecByteBlock client_pub_ecdh(client_pub_ecdh_size);
    recv(client_pub_ecdh.data(), client_pub_ecdh_size);

    // perform agreement
    CryptoPP::SecByteBlock shared_ecdh(dh.AgreedValueLength());
    if (!dh.Agree(shared_ecdh, serv_priv_ecdh, client_pub_ecdh)) {
        // TODO runtime exception
        std::cerr << "Unable to perform ECDH agreement" << std::endl;
        return false;
    }

    // derive stream key
    CryptoPP::HKDF<CryptoPP::SHA256> hkdf;
    CryptoPP::SecByteBlock derived_key(Crypto::KEY_LEN);
    hkdf.DeriveKey(derived_key, sizeof(derived_key), shared_ecdh.BytePtr(),
            shared_ecdh.SizeInBytes(), Crypto::HKDF_SALT, Crypto::HKDF_SALT_LEN,
            Crypto::HKDF_INFO, Crypto::HKDF_INFO_LEN);
    
    // setup stream cipher
    if (gen_iv) {
        rng.GenerateBlock(iv, Crypto::IV_LEN); 
        send(iv, Crypto::IV_LEN);
    } else {
        recv(iv, Crypto::IV_LEN);
    }
    enc.SetKeyWithIV(derived_key, Crypto::KEY_LEN, iv, Crypto::IV_LEN);
    dec.SetKeyWithIV(derived_key, Crypto::KEY_LEN, iv, Crypto::IV_LEN);

    // send confirmation
    CryptoPP::SecByteBlock confirm_send_ct(Crypto::CONFIRM_PHRASE_LEN);
    CryptoPP::SecByteBlock confirm_send_mac(Crypto::MAC_LEN);
    enc.EncryptAndAuthenticate(confirm_send_ct, confirm_send_mac, Crypto::MAC_LEN,
            iv, Crypto::IV_LEN, Crypto::CIP_HEADER, Crypto::CIP_HEADER_LEN,
            reinterpret_cast<const CryptoPP::byte*>(Crypto::CONFIRM_PHRASE),
            Crypto::CONFIRM_PHRASE_LEN);
    send(confirm_send_ct, Crypto::CONFIRM_PHRASE_LEN);
    send(confirm_send_mac, Crypto::MAC_LEN);

    // receive confirmation
    CryptoPP::SecByteBlock confirm_recv_ct(Crypto::CONFIRM_PHRASE_LEN);
    CryptoPP::SecByteBlock confirm_recv_mac(Crypto::MAC_LEN);
    recv(confirm_recv_ct, Crypto::CONFIRM_PHRASE_LEN);
    recv(confirm_recv_mac, Crypto::MAC_LEN);
    CryptoPP::byte confirm_recv_pt[Crypto::CONFIRM_PHRASE_LEN + 1];
    if (!dec.DecryptAndVerify(confirm_recv_pt, confirm_recv_mac, Crypto::MAC_LEN,
            iv, Crypto::IV_LEN,
            Crypto::CIP_HEADER, Crypto::CIP_HEADER_LEN, 
            confirm_recv_ct, Crypto::CONFIRM_PHRASE_LEN)) {
        std::cerr << "Unable to confirm cipher" << std::endl;
        return false;
    }
    confirm_recv_pt[Crypto::CONFIRM_PHRASE_LEN] = '\0';
    if (std::string(reinterpret_cast<char*>(confirm_recv_pt)) != Crypto::CONFIRM_PHRASE) {
        std::cerr << "Incorrect confirmation message" << std::endl;
        return false;
    }

    std::cout << ":)" << std::endl;

    return true;

}

