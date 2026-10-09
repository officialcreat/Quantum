#pragma once

#include <string>

class Wallet {
    private:
    int port;
public:
    Wallet(int port);
    std::string getAddress() const;
    std::string getPublicKey() const;
    std::string sign(const std::string& message) const;

private:
    std::string privateKey;
    std::string publicKey;
    std::string address;

    void save() const;
    bool load();
};