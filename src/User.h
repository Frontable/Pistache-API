#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct User
{
    int id;
    std::string name;
    std::string email;

    std::string passwordHash;
};


inline void to_json(nlohmann::json& j, const User& user)
{
    j = nlohmann::json{
        {"id", user.id},
        {"name", user.name},
        {"email", user.email}
    };
}
