#pragma once

#include <string>

std::string signMessage(
    const std::string& privateKey,
    const std::string& message
);

bool verifySignature(
    const std::string& publicKey,
    const std::string& message,
    const std::string& signature
);