#pragma once

#include <string>
#include <vector>

std::string EncodeBase58(const std::vector<unsigned char>& input);

std::vector<unsigned char> DecodeBase58(const std::string& input);