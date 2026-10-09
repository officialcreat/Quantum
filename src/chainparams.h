#pragma once

#include <cstdint>
#include <string>

namespace ChainParams
{
    // Название сети
    constexpr const char* NETWORK_NAME = "MyCoin";

    // Версия протокола
    constexpr uint32_t PROTOCOL_VERSION = 1;

    // Порт P2P
    constexpr uint16_t DEFAULT_PORT = 9333;

    // Награда за Genesis
    constexpr uint64_t GENESIS_REWARD = 5000000000ULL;

    // Сложность Proof-of-Work
    constexpr uint32_t DIFFICULTY = 4;

    // Префикс адресов (MC...)
    constexpr uint8_t ADDRESS_PREFIX = 0x32;
}





