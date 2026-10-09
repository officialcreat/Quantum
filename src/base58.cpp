#include "base58.h"

#include <algorithm>

static const char* ALPHABET =
    "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

std::string EncodeBase58(const std::vector<unsigned char>& input)
{
    std::string out;

    for (unsigned char b : input)
        out += ALPHABET[b % 58];

    return out;
}

std::vector<unsigned char> DecodeBase58(const std::string& input)
{
    std::vector<unsigned char> out;

    for (char c : input)
    {
        const char* p = std::find(ALPHABET, ALPHABET + 58, c);
        if (p != ALPHABET + 58)
            out.push_back(static_cast<unsigned char>(p - ALPHABET));
    }

    return out;
}





