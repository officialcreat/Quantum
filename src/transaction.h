#pragma once

#include <cstdint>
#include <string>

struct Transaction
{
    std::string from;
    std::string to;

    double amount = 0.0;
    double fee = 0.0;

    std::string txid;

    std::string publicKey;
    std::string signature;

    // Каноническое содержимое транзакции.
    // Не содержит txid и signature.
    std::string serializeForSigning() const;

    // Рассчитывает TXID.
    std::string calculateTxid() const;

    // Подписывает транзакцию.
    bool sign(const std::string& privateKey);

    // Проверяет подпись.
    bool verifySignature() const;

    // Проверяет соответствие txid содержимому транзакции.
    bool verifyTxid() const;
};