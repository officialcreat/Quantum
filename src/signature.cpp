#include "signature.h"

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bio.h>

std::string signMessage(
    const std::string& privateKey,
    const std::string& message)
{
    BIO* bio = BIO_new_mem_buf(privateKey.data(), privateKey.size());

    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();

    EVP_DigestSignInit(ctx, nullptr, EVP_sha256(), nullptr, pkey);

    EVP_DigestSignUpdate(ctx, message.data(), message.size());

    size_t len = 0;
    EVP_DigestSignFinal(ctx, nullptr, &len);

    std::string sig(len, '\0');

    EVP_DigestSignFinal(
        ctx,
        reinterpret_cast<unsigned char*>(&sig[0]),
        &len);

    sig.resize(len);

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    BIO_free(bio);

    return sig;
}

bool verifySignature(
    const std::string& publicKey,
    const std::string& message,
    const std::string& signature)
{
    BIO* bio = BIO_new_mem_buf(publicKey.data(), publicKey.size());

    EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();

    EVP_DigestVerifyInit(ctx, nullptr, EVP_sha256(), nullptr, pkey);

    EVP_DigestVerifyUpdate(ctx, message.data(), message.size());

    int ok = EVP_DigestVerifyFinal(
        ctx,
        (const unsigned char*)signature.data(),
        signature.size());

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    BIO_free(bio);

    return ok == 1;
}