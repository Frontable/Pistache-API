#pragma once

#include <string>
#include <optional>
#include <stdexcept>
#include <cstdint>

#include "IUserRepository.h"
#include "User.h"

struct AuthResult
{
    User user;
    std::string token;
};


class AuthError : public std::runtime_error
{
public:
    explicit AuthError(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

class AuthService
{
public:
    AuthService(IUserRepository& repository, std::string jwtSecret, int64_t jwtExpirySeconds);

    AuthResult registerUser(
        const std::string& name,
        const std::string& email,
        const std::string& password
    );

    AuthResult login(const std::string& email, const std::string& password);

    
    std::optional<int> verifyToken(const std::string& token) const;

private:
    IUserRepository& repository;
    std::string jwtSecret;
    int64_t jwtExpirySeconds;

    std::string issueToken(const User& user) const;
};
