#pragma once

#include <string>

struct KeyPair
{
    std::string privateKey;
    std::string publicKey;
};

class KeyGenerator
{
public:
    static KeyPair generate();
};