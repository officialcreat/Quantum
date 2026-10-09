#pragma once

#include <string>

class Blockchain;
class Wallet;
class P2P;

class RPC
{
public:
    RPC(Blockchain& chain,
        Wallet& wallet,
        P2P& network);

    void execute(const std::string& command);

private:
    Blockchain& blockchain;
    Wallet& walletRef;
    P2P& p2p;

    void showHelp();
    void showInfo();
};