#pragma once

#include <array>
#include <iomanip>
#include <sstream>
#include <string>

class uint256
{
public:
    std::array<unsigned char,32> data{};

    std::string ToHex() const
    {
        std::stringstream ss;

        for(unsigned char b : data)
        {
            ss << std::hex
               << std::setw(2)
               << std::setfill('0')
               << (int)b;
        }

        return ss.str();
    }
};