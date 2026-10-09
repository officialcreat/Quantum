#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include "blockchain.h"
#include "wallet.h"
#include "node.h"
#include "config.h"
int main() {
    SetConsoleOutputCP(65001);
SetConsoleCP(65001);
setlocale(LC_ALL, ".UTF-8");
    int port;
std::cout << "Порт узла: ";
std::cin >> port;
Wallet wallet(port);
Blockchain blockchain;
Node node(port, blockchain);
blockchain.setNode(&node); 
node.start(); 
    std::string host;
int peerPort = 0;

std::cout << "IP соседа (Enter = нет): ";
std::cin.ignore();
std::getline(std::cin, host);

if (!host.empty()) {
    std::cout << "Порт соседа: ";
    std::cin >> peerPort;
    node.connectTo(host, peerPort);
}
blockchain.load();
    while (true) {
        std::cout << "\n==============================\n";
std::cout << "        QUANTUM CORE\n";
std::cout << "          v" << CORE_VERSION << "\n";
std::cout << "==============================\n\n";
        std::cout << "1. Показать адрес\n";
        std::cout << "2. Баланс\n";
        std::cout << "3. Перевод\n";
        std::cout << "4. Показать блокчейн\n";
        std::cout << "5. Майнить блок\n";
        std::cout << "6. Информация о сети\n";
        std::cout << "0. Выход\n";
        std::cout << "Выбор: ";

        int cmd;
        std::cin >> cmd;

        if (cmd == 0)
            break;

        if (cmd == 1)
            std::cout << "Адрес: " << wallet.getAddress() << "\n";

        if (cmd == 2)
            std::cout << "Баланс: "
                      << blockchain.getBalance(wallet.getAddress())
                      << " QTM\n";

        if (cmd == 3) {
    std::string to;
    double amount;

    std::cout << "Кому (QTM адрес): ";
    std::cin >> to;

    std::cout << "Сколько QTM: ";
    std::cin >> amount;

    if (blockchain.send(wallet, to, amount)) {
    std::cout << "Перевод добавлен в mempool!\n";
} else {
    std::cout << "Ошибка перевода!\n";
}
    
}

      if (cmd == 4)
{
    for (const auto& block : blockchain.getChain())
    {
        std::cout << "\n=== BLOCK #" << block.index << " ===\n";
        std::cout << "Hash: " << block.hash << "\n";
        std::cout << "Miner: " << block.minerAddress << "\n";
        std::cout << "TX: " << block.transactions.size() << "\n";
    }
}
if (cmd == 5) {
    blockchain.mineBlock(wallet.getAddress());

    std::cout << "Блок добыт и отправлен в сеть!\n";;
}
if (cmd == 6)
{
    std::cout << "\n===== QUANTUM NETWORK =====\n";
    std::cout << "Монета: " << COIN_NAME << "\n";
    std::cout << "Тикер: " << TICKER << "\n";
    std::cout << "Версия: " << CORE_VERSION << "\n";
    std::cout << "Высота: " << blockchain.height() << "\n";
    std::cout << "Награда: " << BLOCK_REWARD << " QTM\n\n";
    std::cout << "Сложность: " << DIFFICULTY << "\n";
}
} // конец while

return 0;
} // конец main