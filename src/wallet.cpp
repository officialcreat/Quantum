#include "wallet.h"
#include "script.h"
#include <fstream>
#include <filesystem>
#include <random>
static std::string randomHex(size_t len)
{
    const char* h = "0123456789abcdef";

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> d(0,15);

    std::string s;
    for(size_t i=0;i<len;i++)
        s += h[d(rng)];

    return s;
}

Wallet::Wallet(int port)
{
    this->port = port;

    if (load())
        return;
    Script::generateKeyPair(privateKey, publicKey);
   std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 15);

const char* hex = "0123456789ABCDEF";
address = "QTM" + std::to_string(port) + publicKey.substr(0, 4);
    save();
}

std::string Wallet::getAddress() const
{
    return address;
}

std::string Wallet::getPublicKey() const
{
    return publicKey;
}

std::string Wallet::sign(const std::string& message) const
{
    return Script::sign(message, privateKey);
}
void Wallet::save() const
{
    std::ofstream file("wallet_" + std::to_string(port) + ".dat");

    file << privateKey << '\n';
    file << publicKey << '\n';
    file << address << '\n';
}

bool Wallet::load()
{
    std::string filename = "wallet_" + std::to_string(port) + ".dat";

    if (!std::filesystem::exists(filename))
        return false;

    std::ifstream file(filename);

    std::getline(file, privateKey);
    std::getline(file, publicKey);
    std::getline(file, address);

    return !privateKey.empty() &&
           !publicKey.empty() &&
           !address.empty();
}