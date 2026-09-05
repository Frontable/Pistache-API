#include "Config.h"

#include <cstdlib>

namespace
{
    std::string envOr(const char* name, const std::string& fallback)
    {
        const char* value = std::getenv(name);
        return value ? std::string(value) : fallback;
    }
}

Config Config::fromEnv()
{
    Config config;

    config.dbHost = envOr("DB_HOST", config.dbHost);
    config.dbPort = envOr("DB_PORT", config.dbPort);
    config.dbName = envOr("DB_NAME", config.dbName);
    config.dbUser = envOr("DB_USER", config.dbUser);
    config.dbPassword = envOr("DB_PASSWORD", config.dbPassword);

    config.jwtSecret = envOr("JWT_SECRET", config.jwtSecret);

    if (const char* expiry = std::getenv("JWT_EXPIRY_SECONDS"))
    {
        config.jwtExpirySeconds = std::stoll(expiry);
    }

    if (const char* port = std::getenv("PORT"))
    {
        config.port = static_cast<uint16_t>(std::stoi(port));
    }

    if (const char* poolSize = std::getenv("DB_POOL_SIZE"))
    {
        config.dbPoolSize = static_cast<std::size_t>(std::stoul(poolSize));
    }

    return config;
}

std::string Config::connectionString() const
{
    return "host=" + dbHost
        + " port=" + dbPort
        + " dbname=" + dbName
        + " user=" + dbUser
        + " password=" + dbPassword;
}
