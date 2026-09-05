#pragma once

#include <vector>
#include <optional>
#include <string>
#include <stdexcept>

#include "IUserRepository.h"

struct PagedUsers
{
    std::vector<User> users;
    int page;
    int limit;
    int total;
};


class ValidationError : public std::runtime_error
{
public:
    explicit ValidationError(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

class UserService
{
public:
    explicit UserService(IUserRepository& repository);

    PagedUsers getAll(int page, int limit);

    std::optional<User> getById(int id);

    std::optional<User> update(
        int id,
        const std::string& name,
        const std::string& email
    );

    bool remove(int id);

    // Shared by UserService callers and AuthService — kept here so the
    // validation rule lives in exactly one place.
    static void validateNameAndEmail(const std::string& name, const std::string& email);

private:
    IUserRepository& repository;
};
