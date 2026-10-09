#pragma once

#include <cstdint>
#include <string>

class P2P
{
public:
    P2P();

    bool Start(uint16_t port);
    void Stop();

    bool Connect(const std::string& host, uint16_t port);
    bool Broadcast(const std::string& message);

    bool IsRunning() const;

private:
    bool running;
    uint16_t listenPort;
};