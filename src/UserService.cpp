#include "UserService.h"

#include <regex>
#include <algorithm>

UserService::UserService(IUserRepository& repository)
    : repository(repository)
{
}

void UserService::validateNameAndEmail(const std::string& name, const std::string& email)
{
    if (name.empty() || name.size() > 255)
    {
        throw ValidationError("name must be between 1 and 255 characters");
    }

    
    static const std::regex emailPattern(R"([^@\s]+@[^@\s]+\.[^@\s]+)");

    if (!std::regex_match(email, emailPattern))
    {
        throw ValidationError("email must be a valid email address");
    }
}

PagedUsers UserService::getAll(int page, int limit)
{
    page = std::max(page, 1);
    limit = std::clamp(limit, 1, 100);

    int offset = (page - 1) * limit;

    PagedUsers result;
    result.users = repository.getAll(limit, offset);
    result.page = page;
    result.limit = limit;
    result.total = repository.count();

    return result;
}

std::optional<User> UserService::getById(int id)
{
    return repository.getById(id);
}

std::optional<User> UserService::update(
    int id,
    const std::string& name,
    const std::string& email)
{
    validateNameAndEmail(name, email);

    return repository.update(id, name, email);
}

bool UserService::remove(int id)
{
    return repository.remove(id);
}
