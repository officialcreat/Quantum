#pragma once

#include <string>

std::string sha256(const std::string& input);
std::string doubleSHA256(const std::string& input);
std::string hash160(const std::string& input);