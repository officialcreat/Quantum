#include "block.h"
#include "transaction.h"
#include "crypto.h"

#include <ctime>
#include <iomanip>
#include <sstream>

Block::Block()
{
    index = 0;
    timestamp = std::time(nullptr);
    nonce = 0;
}

std::string Block::calculateHash() const
{
    std::ostringstream ss;

    ss << "QTM-BLOCK-V1\n";
    ss << "index=" << index << '\n';
    ss << "timestamp=" << timestamp << '\n';
    ss << "previousHash=" << previousHash << '\n';
    ss << "minerAddress=" << minerAddress << '\n';
    ss << "nonce=" << nonce << '\n';
    ss << "txCount=" << transactions.size() << '\n';

    for (const auto& tx : transactions)
    {
        ss << "tx=" << tx.serializeForSigning() << '\n';
        ss << "signature=" << tx.signature << '\n';
        ss << "txid=" << tx.txid << '\n';
    }

    return sha256(ss.str());
}

void Block::mine(uint32_t difficulty)
{
    if (difficulty >= 64)
        return;

    const std::string target(difficulty, '0');

    do
    {
        ++nonce;
        hash = calculateHash();
    }
    while (hash.compare(0, difficulty, target) != 0);
}