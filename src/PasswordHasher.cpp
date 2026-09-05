#include "PasswordHasher.h"

#include <openssl/rand.h>
#include <openssl/evp.h>

#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <vector>

std::string PasswordHasher::toHex(const unsigned char* data, std::size_t length)
{
    std::ostringstream out;

    out << std::hex << std::setfill('0');

    for (std::size_t i = 0; i < length; ++i)
    {
        out << std::setw(2) << static_cast<int>(data[i]);
    }

    return out.str();
}

std::string PasswordHasher::fromHex(const std::string& hex)
{
    std::string bytes;
    bytes.reserve(hex.size() / 2);

    for (std::size_t i = 0; i + 1 < hex.size(); i += 2)
    {
        bytes.push_back(
            static_cast<char>(std::stoi(hex.substr(i, 2), nullptr, 16))
        );
    }

    return bytes;
}

std::string PasswordHasher::hash(const std::string& password)
{
    unsigned char salt[SaltBytes];

    if (RAND_bytes(salt, SaltBytes) != 1)
    {
        throw std::runtime_error("Failed to generate random salt");
    }

    unsigned char derived[HashBytes];

    int ok = PKCS5_PBKDF2_HMAC(
        password.c_str(), static_cast<int>(password.size()),
        salt, SaltBytes,
        Iterations,
        EVP_sha256(),
        HashBytes, derived
    );

    if (ok != 1)
    {
        throw std::runtime_error("Failed to derive password hash");
    }

    return "pbkdf2$" + std::to_string(Iterations) + "$"
        + toHex(salt, SaltBytes) + "$"
        + toHex(derived, HashBytes);
}

bool PasswordHasher::verify(const std::string& password, const std::string& stored)
{
    
    std::vector<std::string> parts;
    std::stringstream ss(stored);
    std::string part;

    while (std::getline(ss, part, '$'))
    {
        parts.push_back(part);
    }

    if (parts.size() != 4 || parts[0] != "pbkdf2")
    {
        return false;
    }

    int iterations = std::stoi(parts[1]);
    std::string salt = fromHex(parts[2]);
    std::string expectedHex = parts[3];

    unsigned char derived[HashBytes];

    int ok = PKCS5_PBKDF2_HMAC(
        password.c_str(), static_cast<int>(password.size()),
        reinterpret_cast<const unsigned char*>(salt.data()), static_cast<int>(salt.size()),
        iterations,
        EVP_sha256(),
        HashBytes, derived
    );

    if (ok != 1)
    {
        return false;
    }

    std::string actualHex = toHex(derived, HashBytes);

    
    if (actualHex.size() != expectedHex.size())
    {
        return false;
    }

    unsigned char diff = 0;

    for (std::size_t i = 0; i < actualHex.size(); ++i)
    {
        diff |= static_cast<unsigned char>(actualHex[i] ^ expectedHex[i]);
    }

    return diff == 0;
}
