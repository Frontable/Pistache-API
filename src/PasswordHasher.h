#pragma once

#include <string>

//password hashing using OpenSSL.
class PasswordHasher
{
public:
    static std::string hash(const std::string& password);

    static bool verify(const std::string& password, const std::string& stored);

private:
    static constexpr int Iterations = 100000;
    static constexpr int SaltBytes = 16;
    static constexpr int HashBytes = 32;

    static std::string toHex(const unsigned char* data, std::size_t length);
    static std::string fromHex(const std::string& hex);
};
