#pragma once
const std::string COIN_NAME = "Quantum";
const std::string TICKER = "QTM";
const std::string CORE_VERSION = "1.0.0";

const int BLOCK_REWARD = 50;
const int DIFFICULTY = 4;
inline std::string getWalletFile(int port)
{
    return "wallet_" + std::to_string(port) + ".dat";
}