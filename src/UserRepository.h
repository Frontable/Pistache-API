#pragma once

#include <vector>
#include <optional>
#include <string>

#include "IUserRepository.h"
#include "ConnectionPool.h"

class UserRepository : public IUserRepository
{
public:
    explicit UserRepository(ConnectionPool& pool);

    std::vector<User> getAll(int limit, int offset) override;

    int count() override;

    std::optional<User> getById(int id) override;

    std::optional<User> getByEmail(const std::string& email) override;

    User create(
        const std::string& name,
        const std::string& email,
        const std::string& passwordHash
    ) override;

    std::optional<User> update(
        int id,
        const std::string& name,
        const std::string& email
    ) override;

    bool remove(int id) override;

private:
    ConnectionPool& pool;

    static User rowToUser(PGresult* result, int row);
};
