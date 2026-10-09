#include "blockchain.h"
#include "transaction.h"
#include "node.h"
#include "wallet.h"
#include <sstream>
#include <fstream>
#include <ctime>
#include "crypto.h"
#include "signature.h"
#include "message.h"
#include <iostream>
#include "script.h"
#include <filesystem>
#include "config.h"
#include "storage.h"
Blockchain::Blockchain(Node* n) : node(n)
{
    if (!Storage::Load(*this, "blockchain.dat"))
    {
        createGenesisBlock();
    }
}
void Blockchain::setNode(Node* n)
{
    node = n;
}
void Blockchain::createGenesisBlock()
{
    Block genesis;
    genesis.index = 0;
    genesis.previousHash = "0";

    Transaction reward;
    reward.from = "NETWORK";
    reward.to = "QTM500004F9";
    reward.amount = 350;
    reward.fee = 0;
    reward.txid = "genesis";
    reward.publicKey = "";
    reward.signature = "";

    genesis.transactions.push_back(reward);
    genesis.mine(2);

    chain.clear();
    chain.push_back(genesis);

    balances.clear();
    balances["QTM500004F9"] = 350;
}
bool Blockchain::addBlock(const Block& block)
{
    if (chain.empty())
        createGenesisBlock();

    Block newBlock = block;

    newBlock.index = chain.size();
    newBlock.previousHash = chain.back().hash;

    newBlock.mine(DIFFICULTY);

    chain.push_back(newBlock);
    saveChain();

    return true;
}
const Block& Blockchain::latestBlock() const
{
    static Block genesis;

    if (chain.empty()) {
        return genesis;
    }

    return chain.back();
}

size_t Blockchain::height() const
{
    return chain.size();
}
double Blockchain::getBalance(const std::string& address) const
{
    double balance = 0.0;

    // Только подтверждённые транзакции.
    for (const auto& block : chain)
    {
        for (const auto& tx : block.transactions)
        {
            if (tx.to == address)
                balance += tx.amount;

            if (tx.from == address)
                balance -= (tx.amount + tx.fee);
        }
    }

    return balance;
}
bool Blockchain::hasSufficientBalance(const Transaction& tx) const
{
    if (tx.amount <= 0.0)
        return false;

    if (tx.fee < 0.0)
        return false;

    double balance = getBalance(tx.from);

    double pendingSpend = 0.0;

    for (const auto& pending : mempool)
    {
        if (pending.from == tx.from)
            pendingSpend += pending.amount + pending.fee;
    }

    return balance >= pendingSpend + tx.amount + tx.fee;
}
void Blockchain::mineBlock(const std::string& minerAddress)
{
    Transaction reward;
    reward.from = "NETWORK";
    reward.to = minerAddress;
    reward.amount = 50;
    
reward.publicKey = "";
reward.signature = "";
reward.txid = "reward";
std::vector<Transaction> txs = mempool;
txs.insert(txs.begin(), reward);
   Block block;
block.index = chain.size();
block.previousHash = latestBlock().hash;
block.minerAddress = minerAddress;
block.transactions = txs;

for (const Transaction& tx : mempool)
{
    balances[tx.from] -= (tx.amount + tx.fee);
    balances[tx.to] += tx.amount;
}

balances[minerAddress] += reward.amount;

std::cout << "[MINING] Mempool TX count: "
          << mempool.size() << "\n";

for (const auto& tx : block.transactions)
{
    std::cout << "[MINING] TX: "
              << tx.from << " -> "
              << tx.to << " : "
              << tx.amount
              << " QTM\n";
}

block.mine(3);
chain.push_back(block);
mempool.clear();
Storage::Save(*this, "blockchain.dat");
if (node)
{
    node->broadcastBlock(block);
}

std::cout << "Блок добыт и отправлен в сеть!\n";
}
bool Blockchain::send(const Wallet& sender,
                      const std::string& to,
                      double amount)
{
    if (amount <= 0)
    return false;

std::string from = sender.getAddress();

const double FEE = 1.0;

  Transaction tx;

tx.from = from;
tx.to = to;
tx.amount = amount;
tx.fee = FEE;
if (!hasSufficientBalance(tx))
{
    std::cout << "[TX] Недостаточно средств\n";
    std::cout << "[TX] From: " << tx.from << "\n";
    std::cout << "[TX] Balance: " << getBalance(tx.from) << " QTM\n";
    std::cout << "[TX] Amount: " << tx.amount << " QTM\n";
    std::cout << "[TX] Fee: " << tx.fee << " QTM\n";
    return false;
}
tx.publicKey = sender.getPublicKey();

tx.signature = sender.sign(
    tx.serializeForSigning()
);

if (tx.signature.empty())
    return false;

tx.txid = tx.calculateTxid();

if (!tx.verifySignature())
    return false;

if (!tx.verifyTxid())
    return false;
for (const auto& pending : mempool)
{
    if (pending.txid == tx.txid)
        return false;
}
mempool.push_back(tx);

if (node)
{
    node->broadcastTransaction(tx);
}

saveChain();

return true;
}

bool Blockchain::verifyTransaction(const Transaction& tx) const
{
    std::string message =
    tx.from + tx.to +
    std::to_string(tx.amount) +
    tx.publicKey;
    if (!Script::verify(message, tx.signature, tx.publicKey))
{
    std::cout << "SIGNATURE ERROR\n";
    return false;
}

    return true;
}
   bool Blockchain::isChainValid() const
{
    for (size_t i = 1; i < chain.size(); i++)
    {
        const Block& current = chain[i];
        const Block& previous = chain[i - 1];

        if (current.previousHash != previous.hash)
            return false;

        if (current.hash != current.calculateHash())
            return false;

        for (const auto& tx : current.transactions)
        {
            if (!verifyTransaction(tx))
                return false;
        }
    }

    return true;
}
void Blockchain::saveChain()
{
    std::ofstream out("quantum.qtm");

    for (const auto& block : chain)
    {
        out << block.index << '|'
            << block.previousHash << '|'
            << block.hash << '|'
            << block.minerAddress << '|'
            << block.transactions.size() << '\n';

        for (const auto& tx : block.transactions)
        {
            out << tx.from << '|'
    << tx.to << '|'
    << tx.amount << '|'
    << tx.fee << '|'
    << tx.txid << '|'
    << tx.publicKey << '|'
    << tx.signature << '\n';
        }
    }
}

void Blockchain::loadChain()
{ 
    std::cout << "LOAD 1\n";
    std::ifstream in("quantum.qtm");
    if (!in.is_open())
        return;

    chain.clear();
    std::cout << "LOAD 2\n";
balances.clear();

    std::string line;
    while (std::getline(in, line))
    {
        Block block;

        std::stringstream ss(line);
        std::string field;

        std::getline(ss, field, '|');
if (field.empty()) continue;
std::cout << "LINE: " << line << std::endl;
block.index = field.empty() ? 0 : std::stoi(field);

        std::getline(ss, block.previousHash, '|');
        std::getline(ss, block.hash, '|');
        std::string miner;
std::getline(ss, miner, '|');   // NETWORK

std::getline(ss, field, '|');   // количество транзакций
int txCount = field.empty() ? 0 : std::stoi(field);

block.minerAddress = miner;

        for (int i = 0; i < txCount; i++)
{
    std::getline(in, line);
    std::stringstream txs(line);

    Transaction tx;
std::string amount;
std::string fee;

    std::getline(txs, tx.from, '|');
std::getline(txs, tx.to, '|');

std::getline(txs, amount, '|');
tx.amount = amount.empty() ? 0 : std::stod(amount);

std::getline(txs, fee, '|');
tx.fee = fee.empty() ? 0 : std::stod(fee);

std::getline(txs, tx.txid, '|');
std::getline(txs, tx.publicKey, '|');
std::getline(txs, tx.signature);
    block.transactions.push_back(tx);
}

// balances[block.minerAddress] += quantum.qtm;

// все транзакции блока
for (const auto& tx : block.transactions)
{
    if (tx.from == "NETWORK") {
        balances[tx.to] += tx.amount;
    } else {
        balances[tx.from] -= tx.amount;
        balances[tx.to] += tx.amount;
    }
    }
    chain.push_back(block);
}
}
void Blockchain::save() const
{
    std::ofstream file("quantum.qtm");

    file << chain.size() << '\n';

    for (const Block& b : chain) {
    file << b.index << '\n';
    file << b.timestamp << '\n';
    file << b.previousHash << '\n';
    file << b.hash << '\n';
    file << b.nonce << '\n';
    file << b.minerAddress << '\n';
    file << b.transactions.size() << '\n';

    for (const Transaction& tx : b.transactions) {
        file << tx.from << '\n';
        file << tx.to << '\n';
        file << tx.amount << '\n';
        file << tx.publicKey << '\n';
        file << tx.signature << '\n';
        file << tx.txid << '\n';
    }
}
        }
 bool Blockchain::load()
{
    if (!std::filesystem::exists("quantum.qtm"))
        return false;

    std::ifstream file("quantum.qtm");

    size_t blocks;
    file >> blocks;
    file.ignore();

    chain.clear();

    for (size_t i = 0; i < blocks; i++)
    {
        Block b;

        file >> b.index;
file.ignore();

file >> b.timestamp;
file.ignore();

std::getline(file, b.previousHash);
std::getline(file, b.hash);

file >> b.nonce;
file.ignore();

std::getline(file, b.minerAddress);

size_t txCount;
file >> txCount;
file.ignore();
        for (size_t j = 0; j < txCount; j++)
        {
            Transaction tx;

            std::getline(file, tx.from);
            std::getline(file, tx.to);

            file >> tx.amount;
            file.ignore();

            std::getline(file, tx.publicKey);
            std::getline(file, tx.signature);
            std::getline(file, tx.txid);

            b.transactions.push_back(tx);
        }

        chain.push_back(b);
        // восстановить награду майнеру
// balances[b.minerAddress] += quantum.qtm;

// восстановить все транзакции
for (const auto& tx : b.transactions) {
    balances[tx.from] -= tx.amount;
    balances[tx.to] += tx.amount;
}
    }

    return !chain.empty();
}
void Blockchain::addRemoteBlock(const std::string& data)
{
    std::cout << "[SYNC] Raw BLOCK: " << data << "\n";
    std::stringstream ss(data);
    std::string part;
    std::vector<std::string> v;

    while (std::getline(ss, part, '|'))
        v.push_back(part);

    if (v.size() < 7 || v[0] != "BLOCK")
        return;
if (v.size() < 8)
{
    std::cout << "[SYNC] BLOCK rejected: no transaction count\n";
    return;
}

size_t txCount = 0;

try
{
    txCount = std::stoull(v[7]);
}
catch (...)
{
    std::cout << "[SYNC] BLOCK rejected: invalid TX count\n";
    return;
}

if (v.size() != 8 + txCount)
{
    std::cout << "[SYNC] BLOCK rejected: wrong TX count\n";
    std::cout << "[SYNC] Expected fields: "
              << (8 + txCount)
              << ", received: "
              << v.size()
              << "\n";
    return;
}
Block b;
    b.index = std::stoi(v[1]);
    b.timestamp = std::stoll(v[2]);
    b.previousHash = v[3];
    b.hash = v[4];
    b.nonce = std::stoull(v[5]);
    b.minerAddress = v[6];

if (!chain.empty())
{
    const Block& previousBlock = latestBlock();

    if (b.index != previousBlock.index + 1)
    {
        std::cout << "[SYNC] BLOCK rejected: invalid index\n";
        std::cout << "[SYNC] Expected index: "
                  << previousBlock.index + 1 << "\n";
        std::cout << "[SYNC] Received index: "
                  << b.index << "\n";
        return;
    }

    if (b.previousHash != previousBlock.hash)
    {
        std::cout << "[SYNC] BLOCK rejected: invalid previous hash\n";
        std::cout << "[SYNC] Expected previous hash: "
                  << previousBlock.hash << "\n";
        std::cout << "[SYNC] Received previous hash: "
                  << b.previousHash << "\n";
        return;
    }
}

std::cout << "[SYNC] Previous block link OK\n";
uint32_t difficulty = 3;

std::string requiredPrefix(difficulty, '0');

if (b.hash.substr(0, difficulty) != requiredPrefix)
{
    std::cout << "[SYNC] BLOCK rejected: invalid proof of work\n";
    std::cout << "[SYNC] Required prefix: "
              << requiredPrefix << "\n";
    std::cout << "[SYNC] Block hash: "
              << b.hash << "\n";
    return;
}

std::cout << "[SYNC] Proof of Work OK\n";
for (size_t i = 0; i < txCount; ++i)
{
    std::stringstream txss(v[8 + i]);
    std::string field;
    std::vector<std::string> tv;

    while (std::getline(txss, field, ';'))
        tv.push_back(field);

    if (tv.size() != 7)
    {
        std::cout << "[SYNC] BLOCK rejected: invalid TX fields\n";
        return;
    }

    Transaction tx;

    try
    {
        tx.from = tv[0];
        tx.to = tv[1];
        tx.amount = std::stod(tv[2]);
        tx.fee = std::stod(tv[3]);
        tx.publicKey = tv[4];
        tx.signature = tv[5];
        tx.txid = tv[6];
    }
    catch (...)
{
    std::cout << "[SYNC] BLOCK rejected: TX parsing error\n";
    return;
}

if (tx.from == "NETWORK")
{
    if (tx.amount <= 0.0)
    {
        std::cout << "[SYNC] BLOCK rejected: invalid reward\n";
        return;
    }

    if (tx.to != b.minerAddress)
    {
        std::cout << "[SYNC] BLOCK rejected: reward recipient mismatch\n";
        return;
    }

    if (tx.txid != "reward")
    {
        std::cout << "[SYNC] BLOCK rejected: invalid reward TXID\n";
        return;
    }
}
else
{
    if (tx.from.empty() ||
        tx.to.empty() ||
        tx.publicKey.empty() ||
        tx.signature.empty() ||
        tx.txid.empty())
    {
        std::cout << "[SYNC] BLOCK rejected: empty TX field\n";
        return;
    }

    if (tx.amount <= 0.0)
    {
        std::cout << "[SYNC] BLOCK rejected: invalid TX amount\n";
        return;
    }

    if (tx.fee < 0.0)
    {
        std::cout << "[SYNC] BLOCK rejected: invalid TX fee\n";
        return;
    }

    if (!tx.verifyTxid())
    {
        std::cout << "[SYNC] BLOCK rejected: invalid TXID\n";
        return;
    }

    if (!tx.verifySignature())
    {
        std::cout << "[SYNC] BLOCK rejected: invalid TX signature\n";
        return;
    }

    std::cout << "[SYNC] TX validation OK: "
              << tx.txid << "\n";
}

b.transactions.push_back(tx);
}
std::string calculatedHash = b.calculateHash();

if (calculatedHash != b.hash)
{
    std::cout << "[SYNC] BLOCK rejected: invalid hash\n";
    std::cout << "[SYNC] Received hash: "
              << b.hash << "\n";
    std::cout << "[SYNC] Calculated hash: "
              << calculatedHash << "\n";
    return;
}

std::cout << "[SYNC] Block hash OK\n";
   

   if (!chain.empty() && b.hash == latestBlock().hash)
    return;

std::cout << "[SYNC] Adding block #" << b.index << "\n";
std::cout << "[SYNC] Previous hash: " << b.previousHash << "\n";
std::cout << "[SYNC] Block hash: " << b.hash << "\n";
double blockSpend = 0.0;

for (const auto& tx : b.transactions)
{
    if (tx.from == "NETWORK")
        continue;

    blockSpend += tx.amount + tx.fee;
}

std::map<std::string, double> blockSpends;

for (const auto& tx : b.transactions)
{
    if (tx.from == "NETWORK")
        continue;

    blockSpends[tx.from] += tx.amount + tx.fee;
}

for (const auto& [address, spend] : blockSpends)
{
    double balance = getBalance(address);

    if (spend > balance)
    {
        std::cout << "[SYNC] BLOCK rejected: insufficient balance\n";
        std::cout << "[SYNC] Address: "
                  << address << "\n";
        std::cout << "[SYNC] Balance: "
                  << balance << "\n";
        std::cout << "[SYNC] Required: "
                  << spend << "\n";
        return;
    }
}

std::cout << "[SYNC] Block balance validation OK\n";
for (const auto& tx : b.transactions)
{
    if (tx.from == "NETWORK")
    {
        balances[tx.to] += tx.amount;
        continue;
    }

    balances[tx.from] -= (tx.amount + tx.fee);
    balances[tx.to] += tx.amount;
}
chain.push_back(b);

std::cout << "[SYNC] Chain size = "
          << chain.size() << std::endl;
    std::cout << "[SYNC] Chain size = " << chain.size() << std::endl;

    save();

    std::cout << "\n[SYNC] Block #" << b.index
              << " accepted from network!\n";
}
void Blockchain::addRemoteTransaction(const std::string& data)
{
    std::cout << "[SYNC] Raw TX: " << data << "\n";
    std::stringstream ss(data);
    std::string part;
    std::vector<std::string> v;
    while (std::getline(ss, part, '|'))
        v.push_back(part);
    // TX|from|to|amount|fee|publicKey|signature|txid
    if (v.size() != 8)
    {
        std::cout << "[SYNC] TX rejected: wrong field count = "
                  << v.size() << "\n";
        return;
    }
    if (v[0] != "TX")
    {
        std::cout << "[SYNC] TX rejected: invalid prefix: "
                  << v[0] << "\n";
        return;
    }
    Transaction tx;
    try
    {
        tx.from = v[1];
        tx.to = v[2];
        tx.amount = std::stod(v[3]);
        tx.fee = std::stod(v[4]);
        tx.publicKey = v[5];
        tx.signature = v[6];
        tx.txid = v[7];
    }
    catch (...)
    {
        std::cout << "[SYNC] TX rejected: parsing error\n";
        return;
    }
    std::cout << "[SYNC] TX parsed successfully\n";
    // Базовая проверка полей.
    if (tx.from.empty() ||
        tx.to.empty() ||
        tx.publicKey.empty() ||
        tx.signature.empty() ||
        tx.txid.empty())
    {
        std::cout << "[SYNC] TX rejected: empty field\n";
        return;
    }
    std::cout << "[SYNC] Fields OK\n";
    if (tx.amount <= 0.0)
    {
        std::cout << "[SYNC] TX rejected: invalid amount\n";
        return;
    }
    if (tx.fee < 0.0)
    {
        std::cout << "[SYNC] TX rejected: invalid fee\n";
        return;
    }
    std::cout << "[SYNC] Amount/Fee OK\n";
    // TXID должен соответствовать содержимому транзакции.
    if (!tx.verifyTxid())
    {
        std::cout << "[SYNC] TX rejected: invalid TXID\n";
        return;
    }
    std::cout << "[SYNC] TXID OK: "
              << tx.txid << "\n";
    // Подпись должна соответствовать транзакции.
    if (!tx.verifySignature())
    {
        std::cout << "[SYNC] TX rejected: invalid signature\n";
        return;
    }
    std::cout << "[SYNC] Signature OK\n";
    // Защита от дубликатов.
    for (const auto& t : mempool)
    {
        if (t.txid == tx.txid)
        {
            std::cout << "[SYNC] TX rejected: duplicate TXID\n";
            return;
        }
    }
    mempool.push_back(tx);
    std::cout << "[SYNC] TX received: "
              << tx.from << " -> "
              << tx.to << " : "
              << tx.amount
              << " QTM\n";
}

void Blockchain::clear()
{
    chain.clear();
    balances.clear();
}

void Blockchain::addLoadedBlock(const Block& block)
{
    chain.push_back(block);

    for (const auto& tx : block.transactions)
    {
        if (tx.from == "NETWORK")
        {
            balances[tx.to] += tx.amount;
        }
        else
        {
            balances[tx.to] += tx.amount;
            balances[tx.from] -= (tx.amount + tx.fee);
        }
    }
}
void Blockchain::setChain(const std::vector<Block>& newChain)
{
    chain = newChain;
}
const std::vector<Block>& Blockchain::getChain() const
{
    return chain;
}
