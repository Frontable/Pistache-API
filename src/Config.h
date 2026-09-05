#pragma once

#include <string>
#include <cstdint>

struct Config
{
    std::string dbHost = "localhost";
    std::string dbPort = "5432";
    std::string dbName = "pistache_api";
    std::string dbUser = "pistache_user";
    std::string dbPassword = "pistache_password";

    std::string jwtSecret = "dev-secret-change-me";
    int64_t jwtExpirySeconds = 3600; // 1 hour

    uint16_t port = 9080;
    std::size_t dbPoolSize = 8;

    static Config fromEnv();

    std::string connectionString() const;
};
