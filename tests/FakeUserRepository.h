#pragma once

#include <algorithm>
#include <map>

#include "IUserRepository.h"


class FakeUserRepository : public IUserRepository
{
public:
    std::vector<User> getAll(int limit, int offset) override
    {
        std::vector<User> all;

        for (auto& [id, user] : usersById)
        {
            all.push_back(user);
        }

        std::sort(all.begin(), all.end(), [](const User& a, const User& b) { return a.id < b.id; });

        std::vector<User> page;

        for (int i = offset; i < static_cast<int>(all.size()) && static_cast<int>(page.size()) < limit; ++i)
        {
            page.push_back(all[i]);
        }

        return page;
    }

    int count() override
    {
        return static_cast<int>(usersById.size());
    }

    std::optional<User> getById(int id) override
    {
        auto it = usersById.find(id);

        if (it == usersById.end())
        {
            return std::nullopt;
        }

        return it->second;
    }

    std::optional<User> getByEmail(const std::string& email) override
    {
        for (auto& [id, user] : usersById)
        {
            if (user.email == email)
            {
                return user;
            }
        }

        return std::nullopt;
    }

    User create(
        const std::string& name,
        const std::string& email,
        const std::string& passwordHash) override
    {
        User user;
        user.id = nextId++;
        user.name = name;
        user.email = email;
        user.passwordHash = passwordHash;

        usersById[user.id] = user;

        return user;
    }

    std::optional<User> update(
        int id,
        const std::string& name,
        const std::string& email) override
    {
        auto it = usersById.find(id);

        if (it == usersById.end())
        {
            return std::nullopt;
        }

        it->second.name = name;
        it->second.email = email;

        return it->second;
    }

    bool remove(int id) override
    {
        return usersById.erase(id) > 0;
    }

private:
    std::map<int, User> usersById;
    int nextId = 1;
};
