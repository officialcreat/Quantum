#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "transaction.h" 
struct Block
{
    uint32_t index;
    long timestamp;

    std::vector<Transaction> transactions;

    std::string previousHash;
    std::string minerAddress;
    std::string hash;

    uint64_t nonce;

    Block();

    std::string calculateHash() const;
    void mine(uint32_t difficulty);
};