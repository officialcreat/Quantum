#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include "node.h"
#include "blockchain.h"
#include "message.h"
#include <thread>
#pragma comment(lib, "ws2_32.lib")
Node::Node(int p, Blockchain& bc)
    : port(p),
      blockchain(bc),
      serverSocket(INVALID_SOCKET)
{
}

bool Node::start()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
        return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&addr, sizeof(addr)) != 0)
    {
        std::cout << "Bind failed\n";
        return false;
    }

    if (listen(serverSocket, 5) != 0)
    {
        std::cout << "Listen failed\n";
        return false;
    }

    std::thread([this]() {
    while (true)
    {
        SOCKET client = accept(serverSocket, nullptr, nullptr);
        if (client == INVALID_SOCKET)
            continue;

        peers.push_back(client);
        std::cout << "[P2P] Peer connected\n";

        char buf[1024];

while (true)
{
    int n = recv(client, buf, sizeof(buf) - 1, 0);
    if (n <= 0) break;

    buf[n] = '\0';
    std::string data(buf);

    if (data.rfind("BLOCK|", 0) == 0)
    {
        std::cout << "\n[P2P] Новый блок получен\n";
        blockchain.addRemoteBlock(data);
        continue;
    }
    else if (data.rfind("TX|", 0) == 0)
    {
        std::cout << "\n[P2P] Получена транзакция\n";
        blockchain.addRemoteTransaction(data);
        continue;
    }

    std::cout << "[RECEIVED] " << data << std::endl;
}

        closesocket(client);
    }
}).detach();
    std::cout << "Node listening on port " << port << std::endl;
    return true;
}

bool Node::connectTo(const std::string& host, int p)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(p);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) != 0)
    {
        closesocket(sock);
        std::cout << "Connection failed.\n";
        return false;
    }

    peers.push_back(sock);
    send(sock, "HELLO", 5, 0);
std::thread([this, sock]() {
    char buf[1024];

   while (true)
{
    int n = recv(sock, buf, sizeof(buf) - 1, 0);
    if (n <= 0) break;

    buf[n] = '\0';
    std::string data(buf);

    if (data.rfind("BLOCK|", 0) == 0)
    {
        std::cout << "\n[P2P] Новый блок получен\n";
        blockchain.addRemoteBlock(data);
        continue;
    }
    else if (data.rfind("TX|", 0) == 0)
    {
        std::cout << "\n[P2P] Получена транзакция\n";
        blockchain.addRemoteTransaction(data);
        continue;
    }

    std::cout << "[RECEIVED] " << data << std::endl;
}
    closesocket(sock);
}).detach();
    std::cout << "Connected!\n";
    return true;
}
std::string serialize(const Message& msg)
{
    return std::to_string((int)msg.type) + "|" + msg.data;
}

Message deserialize(const std::string& str)
{
    auto p = str.find('|');

    return {
        (MessageType)std::stoi(str.substr(0, p)),
        str.substr(p + 1)
    };
}

void Node::broadcast(const Message& msg)
{
    std::string data = serialize(msg);

    for (SOCKET s : peers) {
        send(s, data.c_str(), data.size(), 0);
    }

    std::cout << "Broadcast: " << data << std::endl;
}
void Node::broadcast(const std::string& data)
{
    for (SOCKET peer : peers)
    {
        send(peer, data.c_str(), data.size(), 0);
    }

    std::cout << "[P2P] Sent: " << data << std::endl;
}
void Node::broadcastBlock(const Block& block)
{
    std::string data =
        "BLOCK|" +
        std::to_string(block.index) + "|" +
        std::to_string(block.timestamp) + "|" +
        block.previousHash + "|" +
        block.hash + "|" +
        std::to_string(block.nonce) + "|" +
        block.minerAddress + "|" +
        std::to_string(block.transactions.size());

    for (const auto& tx : block.transactions)
    {
        data += "|" +
            tx.from + ";" +
            tx.to + ";" +
            std::to_string(tx.amount) + ";" +
            std::to_string(tx.fee) + ";" +
            tx.publicKey + ";" +
            tx.signature + ";" +
            tx.txid;
    }

    broadcast(data);
}
void Node::broadcastTransaction(const Transaction& tx)
{
   std::string data =
    "TX|" +
    tx.from + "|" +
    tx.to + "|" +
    std::to_string(tx.amount) + "|" +
    std::to_string(tx.fee) + "|" +
    tx.publicKey + "|" +
    tx.signature + "|" +
    tx.txid;

    broadcast(data);
}