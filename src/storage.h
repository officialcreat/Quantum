#pragma once

#include <string>
#include "blockchain.h"

class Storage
{
public:
    static bool Save(const Blockchain& chain,
                     const std::string& filename);

    static bool Load(Blockchain& chain,
                     const std::string& filename);
};