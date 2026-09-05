#include "Jwt.h"

#include <openssl/hmac.h>
#include <openssl/evp.h>

#include <chrono>
#include <algorithm>
#include <cstring>
#include <vector>

namespace
{
    int64_t nowSeconds()
    {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
}

std::string Jwt::base64UrlEncode(const std::string& input)
{
    // Standard base64 is 4/3 the input size, plus room for padding/null.
    std::vector<unsigned char> buffer(4 * ((input.size() + 2) / 3) + 1);

    int length = EVP_EncodeBlock(
        buffer.data(),
        reinterpret_cast<const unsigned char*>(input.data()),
        static_cast<int>(input.size())
    );

    std::string encoded(reinterpret_cast<char*>(buffer.data()), length);

    // base64 -> base64url: '+'->'-', '/'->'_', strip '=' padding.
    std::replace(encoded.begin(), encoded.end(), '+', '-');
    std::replace(encoded.begin(), encoded.end(), '/', '_');

    while (!encoded.empty() && encoded.back() == '=')
    {
        encoded.pop_back();
    }

    return encoded;
}

std::optional<std::string> Jwt::base64UrlDecode(const std::string& input)
{
    std::string standard = input;

    std::replace(standard.begin(), standard.end(), '-', '+');
    std::replace(standard.begin(), standard.end(), '_', '/');

    while (standard.size() % 4 != 0)
    {
        standard.push_back('=');
    }

    std::vector<unsigned char> buffer(standard.size());

    int length = EVP_DecodeBlock(
        buffer.data(),
        reinterpret_cast<const unsigned char*>(standard.data()),
        static_cast<int>(standard.size())
    );

    if (length < 0)
    {
        return std::nullopt;
    }

    
    std::size_t padding = 0;
    std::size_t i = standard.size();

    while (i > 0 && input.size() < standard.size() && standard[--i] == '=')
    {
        ++padding;
    }

    std::string decoded(reinterpret_cast<char*>(buffer.data()), length - static_cast<int>(padding));

    return decoded;
}

std::string Jwt::hmacSha256(const std::string& data, const std::string& secret)
{
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLength = 0;

    HMAC(
        EVP_sha256(),
        secret.data(), static_cast<int>(secret.size()),
        reinterpret_cast<const unsigned char*>(data.data()), data.size(),
        digest, &digestLength
    );

    return std::string(reinterpret_cast<char*>(digest), digestLength);
}

std::string Jwt::sign(nlohmann::json payload, const std::string& secret, int64_t expirySeconds)
{
    nlohmann::json header = {
        {"alg", "HS256"},
        {"typ", "JWT"}
    };

    int64_t now = nowSeconds();
    payload["iat"] = now;
    payload["exp"] = now + expirySeconds;

    std::string headerPart = base64UrlEncode(header.dump());
    std::string payloadPart = base64UrlEncode(payload.dump());

    std::string signingInput = headerPart + "." + payloadPart;
    std::string signature = base64UrlEncode(hmacSha256(signingInput, secret));

    return signingInput + "." + signature;
}

std::optional<nlohmann::json> Jwt::verify(const std::string& token, const std::string& secret)
{
    std::size_t firstDot = token.find('.');
    std::size_t secondDot = token.find('.', firstDot == std::string::npos ? 0 : firstDot + 1);

    if (firstDot == std::string::npos || secondDot == std::string::npos)
    {
        return std::nullopt;
    }

    std::string headerPart = token.substr(0, firstDot);
    std::string payloadPart = token.substr(firstDot + 1, secondDot - firstDot - 1);
    std::string signaturePart = token.substr(secondDot + 1);

    std::string signingInput = headerPart + "." + payloadPart;
    std::string expectedSignature = base64UrlEncode(hmacSha256(signingInput, secret));

    
    if (expectedSignature.size() != signaturePart.size())
    {
        return std::nullopt;
    }

    unsigned char diff = 0;

    for (std::size_t i = 0; i < expectedSignature.size(); ++i)
    {
        diff |= static_cast<unsigned char>(expectedSignature[i] ^ signaturePart[i]);
    }

    if (diff != 0)
    {
        return std::nullopt;
    }

    auto decodedPayload = base64UrlDecode(payloadPart);

    if (!decodedPayload)
    {
        return std::nullopt;
    }

    nlohmann::json payload;

    try
    {
        payload = nlohmann::json::parse(*decodedPayload);
    }
    catch (const nlohmann::json::exception&)
    {
        return std::nullopt;
    }

    if (!payload.contains("exp") || !payload["exp"].is_number_integer())
    {
        return std::nullopt;
    }

    if (payload["exp"].get<int64_t>() < nowSeconds())
    {
        return std::nullopt; // expired
    }

    return payload;
}
