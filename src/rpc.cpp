#include "rpc.h"

#include <iostream>

#include "blockchain.h"
#include "wallet.h"
#include "p2p.h"

RPC::RPC(Blockchain& chain,
         Wallet& wallet,
         P2P& network)
    : blockchain(chain),
      walletRef(wallet),
      p2p(network)
{
}

void RPC::execute(const std::string& command)
{
    if(command == "help")
    {
        showHelp();
    }
    else if(command == "getinfo")
    {
        showInfo();
    }
    else
    {
        std::cout << "Unknown command\n";
    }
}

void RPC::showHelp()
{
    std::cout << "\n=== MyCoin RPC ===\n";
    std::cout << "help\n";
    std::cout << "getinfo\n";
    std::cout << "exit\n\n";
}

void RPC::showInfo()
{
    std::cout << "\nHeight : "
              << blockchain.height()
              << "\n";

    std::cout << "Address: "
              << walletRef.getAddress()
              << "\n";

    std::cout << "P2P: "
              << (p2p.IsRunning() ? "running" : "stopped")
              << "\n\n";
}



