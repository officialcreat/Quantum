#include "crypto.h"

#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

std::string sha256(const std::string& input)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256(
        reinterpret_cast<const unsigned char*>(input.data()),
        input.size(),
        hash
    );

    std::stringstream ss;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        ss << std::hex
           << std::setw(2)
           << std::setfill('0')
           << static_cast<int>(hash[i]);
    }

    return ss.str();
}

std::string doubleSHA256(const std::string& input)
{
    return sha256(sha256(input));
}

std::string hash160(const std::string& input)
{
    return sha256(input).substr(0, 40);
}