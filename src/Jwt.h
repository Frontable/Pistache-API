#pragma once

#include <string>
#include <optional>
#include <cstdint>

#include <nlohmann/json.hpp>

// Minimal HS256 JSON Web Token implementation
class Jwt
{
public:
    static std::string sign(
        nlohmann::json payload,
        const std::string& secret,
        int64_t expirySeconds
    );

   
    static std::optional<nlohmann::json> verify(
        const std::string& token,
        const std::string& secret
    );

private:
    static std::string base64UrlEncode(const std::string& input);
    static std::optional<std::string> base64UrlDecode(const std::string& input);
    static std::string hmacSha256(const std::string& data, const std::string& secret);
};
