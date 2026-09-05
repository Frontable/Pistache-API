#include "AuthService.h"

#include "PasswordHasher.h"
#include "Jwt.h"
#include "UserService.h"

AuthService::AuthService(IUserRepository& repository, std::string jwtSecret, int64_t jwtExpirySeconds)
    : repository(repository), jwtSecret(std::move(jwtSecret)), jwtExpirySeconds(jwtExpirySeconds)
{
}

std::string AuthService::issueToken(const User& user) const
{
    nlohmann::json payload = {
        {"sub", user.id},
        {"email", user.email}
    };

    return Jwt::sign(payload, jwtSecret, jwtExpirySeconds);
}

AuthResult AuthService::registerUser(
    const std::string& name,
    const std::string& email,
    const std::string& password)
{
    UserService::validateNameAndEmail(name, email);

    if (password.size() < 8)
    {
        throw ValidationError("password must be at least 8 characters");
    }

    if (repository.getByEmail(email).has_value())
    {
        throw AuthError("an account with that email already exists");
    }

    User user = repository.create(name, email, PasswordHasher::hash(password));

    return AuthResult{ user, issueToken(user) };
}

AuthResult AuthService::login(const std::string& email, const std::string& password)
{
    auto user = repository.getByEmail(email);

    
    if (!user || !PasswordHasher::verify(password, user->passwordHash))
    {
        throw AuthError("invalid email or password");
    }

    return AuthResult{ *user, issueToken(*user) };
}

std::optional<int> AuthService::verifyToken(const std::string& token) const
{
    auto payload = Jwt::verify(token, jwtSecret);

    if (!payload || !payload->contains("sub") || !(*payload)["sub"].is_number_integer())
    {
        return std::nullopt;
    }

    return (*payload)["sub"].get<int>();
}
