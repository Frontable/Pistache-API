#pragma once

#include <vector>
#include <optional>
#include <string>

#include "User.h"


class IUserRepository
{
public:
    virtual ~IUserRepository() = default;

    virtual std::vector<User> getAll(int limit, int offset) = 0;

    virtual int count() = 0;

    virtual std::optional<User> getById(int id) = 0;

    virtual std::optional<User> getByEmail(const std::string& email) = 0;

    // name/email/plaintext-free — repository never sees a plaintext
    // password except already hashed by AuthService.
    virtual User create(
        const std::string& name,
        const std::string& email,
        const std::string& passwordHash
    ) = 0;

    virtual std::optional<User> update(
        int id,
        const std::string& name,
        const std::string& email
    ) = 0;

    virtual bool remove(int id) = 0;
};
