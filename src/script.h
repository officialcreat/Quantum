#pragma once
#include <string>

class Script {
public:
    static std::string sha256(const std::string& data);

    static void generateKeyPair(
        std::string& privateKey,
        std::string& publicKey);

    static std::string sign(
        const std::string& message,
        const std::string& privateKey);

    static bool verify(
        const std::string& message,
        const std::string& signature,
        const std::string& publicKey);
};