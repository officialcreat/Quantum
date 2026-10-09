#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#include <vector>
#include <string>
#include "message.h"

class Blockchain;
class Transaction;
struct Block;
class Node {
public:
    Node(int port, Blockchain& bc);
    bool start();
    bool connectTo(const std::string& host, int port);
    void broadcast(const Message& msg);
    void broadcast(const std::string& data);
void broadcastBlock(const Block& block);
void broadcastTransaction(const Transaction& tx);
private:
    int port;
    Blockchain& blockchain;
    SOCKET serverSocket;
    std::vector<SOCKET> peers;
};