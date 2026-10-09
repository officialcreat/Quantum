#include "storage.h"

#include <fstream>

namespace
{
    constexpr const char* STORAGE_MAGIC = "QTM_STORAGE_V2";
    constexpr std::size_t MAX_BLOCKS = 1000000;
    constexpr std::size_t MAX_TRANSACTIONS_PER_BLOCK = 1000000;
}

bool Storage::Save(const Blockchain& chain,
                   const std::string& filename)
{
    std::ofstream file(filename, std::ios::out | std::ios::trunc);

    if (!file)
        return false;

    const auto& blocks = chain.getChain();

    file << STORAGE_MAGIC << '\n';
    file << blocks.size() << '\n';

    for (const auto& block : blocks)
    {
        file << block.index << '\n';
        file << block.timestamp << '\n';
        file << block.previousHash << '\n';
        file << block.minerAddress << '\n';
        file << block.hash << '\n';
        file << block.nonce << '\n';

        file << block.transactions.size() << '\n';

        for (const auto& tx : block.transactions)
        {
            file << tx.from << '\n';
            file << tx.to << '\n';
            file << tx.amount << '\n';
            file << tx.fee << '\n';
            file << tx.txid << '\n';
            file << (tx.publicKey.empty() ? "-" : tx.publicKey) << '\n';
            file << (tx.signature.empty() ? "-" : tx.signature) << '\n';
        }
    }

    return file.good();
}

bool Storage::Load(Blockchain& chain,
                   const std::string& filename)
{
    std::ifstream file(filename);

    if (!file)
        return false;

    std::string magic;

    if (!(file >> magic))
        return false;

    if (magic != STORAGE_MAGIC)
        return false;

    std::size_t blockCount = 0;

    if (!(file >> blockCount))
        return false;

    if (blockCount > MAX_BLOCKS)
        return false;

    chain.clear();

    for (std::size_t i = 0; i < blockCount; ++i)
    {
        Block block;

        if (!(file >> block.index))
            return false;

        if (!(file >> block.timestamp))
            return false;

        if (!(file >> block.previousHash))
            return false;

        if (!(file >> block.minerAddress))
            return false;

        if (!(file >> block.hash))
            return false;

        if (!(file >> block.nonce))
            return false;

        std::size_t txCount = 0;

        if (!(file >> txCount))
            return false;

        if (txCount > MAX_TRANSACTIONS_PER_BLOCK)
            return false;

        for (std::size_t j = 0; j < txCount; ++j)
        {
            Transaction tx;

            if (!(file >> tx.from))
                return false;

            if (!(file >> tx.to))
                return false;

            if (!(file >> tx.amount))
                return false;

            if (!(file >> tx.fee))
                return false;

            if (!(file >> tx.txid))
                return false;

            if (!(file >> tx.publicKey))
                return false;

            if (!(file >> tx.signature))
                return false;

            if (tx.publicKey == "-")
                tx.publicKey.clear();

            if (tx.signature == "-")
                tx.signature.clear();

            block.transactions.push_back(tx);
        }

        chain.addLoadedBlock(block);
    }

    return file.good() || file.eof();
}