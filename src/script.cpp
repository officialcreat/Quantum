#include "script.h"
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <iostream>
std::string Script::sha256(const std::string& data)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256(
        reinterpret_cast<const unsigned char*>(data.c_str()),
        data.size(),
        hash
    );

    std::stringstream ss;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex
           << std::setw(2)
           << std::setfill('0')
           << (int)hash[i];
    }

    return ss.str();
}
  std::string Script::sign(const std::string& message,
                         const std::string& privateKey)
{
    EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);

    BIGNUM* priv = nullptr;
    BN_hex2bn(&priv, privateKey.c_str());

    EC_KEY_set_private_key(key, priv);

    const EC_GROUP* group = EC_KEY_get0_group(key);
    EC_POINT* pub = EC_POINT_new(group);

    EC_POINT_mul(group, pub, priv, nullptr, nullptr, nullptr);
    EC_KEY_set_public_key(key, pub);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(
    reinterpret_cast<const unsigned char*>(message.data()),
    message.size(),
    hash
);

    unsigned int sigLen = ECDSA_size(key);
    unsigned char* sig = new unsigned char[sigLen];

    ECDSA_sign(0, hash, SHA256_DIGEST_LENGTH,
               sig, &sigLen, key);

    std::stringstream ss;
    for (unsigned int i = 0; i < sigLen; i++)
    {
        ss << std::hex
           << std::setw(2)
           << std::setfill('0')
           << (int)sig[i];
    }

    delete[] sig;
    EC_POINT_free(pub);
    BN_free(priv);
    EC_KEY_free(key);

    return ss.str();
}
bool Script::verify(const std::string& message,
                    const std::string& signature,
                    const std::string& publicKey)
{
    EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);

const EC_GROUP* group = EC_KEY_get0_group(key);
EC_POINT* pub = EC_POINT_hex2point(
    group,
    publicKey.c_str(),
    nullptr,
    nullptr
);

if (pub == nullptr) {
    EC_KEY_free(key);
    return false;
}


if (EC_KEY_set_public_key(key, pub) != 1) {
    EC_POINT_free(pub);
    EC_KEY_free(key);
    return false;
}

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(
    reinterpret_cast<const unsigned char*>(message.data()),
    message.size(),
    hash
);

    std::vector<unsigned char> sig(signature.size()/2);

    for (size_t i = 0; i < sig.size(); i++)
    {
        std::string byte = signature.substr(i*2, 2);
        sig[i] = (unsigned char)strtol(byte.c_str(), nullptr, 16);
    }

    int ok = ECDSA_verify(
    0,
    hash,
    SHA256_DIGEST_LENGTH,
    sig.data(),
    static_cast<int>(sig.size()),
    key
);

EC_POINT_free(pub);
EC_KEY_free(key);

return ok == 1;
}  
void Script::generateKeyPair(std::string& privateKey,
                             std::string& publicKey)
{
    EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);
    EC_KEY_generate_key(key);

    const BIGNUM* priv = EC_KEY_get0_private_key(key);

    char* privHex = BN_bn2hex(priv);
    privateKey = privHex;
    OPENSSL_free(privHex);

    const EC_POINT* pub = EC_KEY_get0_public_key(key);
    const EC_GROUP* group = EC_KEY_get0_group(key);

    char* pubHex = EC_POINT_point2hex(
        group, pub, POINT_CONVERSION_UNCOMPRESSED, nullptr);

    publicKey = pubHex;
    OPENSSL_free(pubHex);

    EC_KEY_free(key);
}
