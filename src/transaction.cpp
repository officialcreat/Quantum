#include "transaction.h"

#include "crypto.h"
#include "script.h"

#include <iomanip>
#include <sstream>

std::string Transaction::serializeForSigning() const
{
    std::ostringstream ss;

    ss << "QTM-TX-V1\n";
    ss << "from=" << from << '\n';
    ss << "to=" << to << '\n';
    ss << "amount=" << std::setprecision(17) << amount << '\n';
    ss << "fee=" << std::setprecision(17) << fee << '\n';
    ss << "publicKey=" << publicKey << '\n';

    return ss.str();
}

std::string Transaction::calculateTxid() const
{
    return doubleSHA256(serializeForSigning());
}

bool Transaction::sign(const std::string& privateKey)
{
    if (privateKey.empty())
        return false;

    // TXID рассчитывается ДО подписи.
    txid = calculateTxid();

    signature = Script::sign(
        serializeForSigning(),
        privateKey
    );

    return !signature.empty();
}

bool Transaction::verifySignature() const
{
    if (publicKey.empty() || signature.empty())
        return false;

    return Script::verify(
        serializeForSigning(),
        signature,
        publicKey
    );
}

bool Transaction::verifyTxid() const
{
    if (txid.empty())
        return false;

    return txid == calculateTxid();
}