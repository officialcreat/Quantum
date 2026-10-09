#include "p2p.h"

P2P::P2P()
{
    running = false;
    listenPort = 0;
}

bool P2P::Start(uint16_t port)
{
    listenPort = port;
    running = true;
    return true;
}

void P2P::Stop()
{
    running = false;
}

bool P2P::Connect(const std::string&, uint16_t)
{
    return true;
}

bool P2P::Broadcast(const std::string&)
{
    return running;
}

bool P2P::IsRunning() const
{
    return running;
}





