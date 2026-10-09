#pragma once

#include "block.h"
#include "transaction.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class Wallet;
class Node;

class Blockchain
{
public:
    explicit Blockchain(Node* node = nullptr);

    const std::vector<Block>& getChain() const;

    void clear();
    void addLoadedBlock(const Block& block);

    bool load();
    void save() const;

    void setNode(Node* n);
    void setChain(const std::vector<Block>& newChain);

    void createGenesisBlock();

    bool addBlock(const Block& block);
    void addRemoteBlock(const std::string& data);
    void addRemoteTransaction(const std::string& data);

    const Block& latestBlock() const;
    size_t height() const;

    void mineBlock(const std::string& minerAddress);

    double getBalance(const std::string& address) const;

    bool send(
        const Wallet& sender,
        const std::string& to,
        double amount
    );

    bool verifyTransaction(const Transaction& tx) const;
    bool hasSufficientBalance(const Transaction& tx) const;
    bool isChainValid() const;

    void saveChain();
    void loadChain();

private:
    Node* node = nullptr;

    std::vector<Block> chain;
    std::map<std::string, uint64_t> balances;
    std::vector<Transaction> mempool;
};