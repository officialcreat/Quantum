#include "keypair.h"
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/bio.h>

KeyPair KeyGenerator::generate()
{
    KeyPair kp;

    EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);
    EC_KEY_generate_key(key);

    BIO* priv = BIO_new(BIO_s_mem());
    PEM_write_bio_ECPrivateKey(priv, key, nullptr, nullptr, 0, nullptr, nullptr);

    char* privData;
    long privLen = BIO_get_mem_data(priv, &privData);
    kp.privateKey.assign(privData, privLen);

    BIO* pub = BIO_new(BIO_s_mem());
    PEM_write_bio_EC_PUBKEY(pub, key);

    char* pubData;
    long pubLen = BIO_get_mem_data(pub, &pubData);
    kp.publicKey.assign(pubData, pubLen);

    BIO_free(priv);
    BIO_free(pub);
    EC_KEY_free(key);

    return kp;
}