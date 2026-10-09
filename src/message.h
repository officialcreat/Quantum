#pragma once
#include <string>
enum class MessageType {
    HELLO = 0,
    BLOCK = 1,
    TX = 2
};

struct Message {
    MessageType type;
    std::string data;
};
std::string serialize(const Message& msg);
Message deserialize(const std::string& str);