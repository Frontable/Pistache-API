#include "UserRepository.h"

#include <stdexcept>
#include <array>

UserRepository::UserRepository(ConnectionPool& pool)
    : pool(pool)
{
}

User UserRepository::rowToUser(PGresult* result, int row)
{
    User user;

    user.id = std::stoi(PQgetvalue(result, row, 0));
    user.name = PQgetvalue(result, row, 1);
    user.email = PQgetvalue(result, row, 2);

    // password_hash is only present when the query selected it.
    if (PQnfields(result) > 3)
    {
        user.passwordHash = PQgetvalue(result, row, 3);
    }

    return user;
}

std::vector<User> UserRepository::getAll(int limit, int offset)
{
    PooledConnection guard(pool);

    std::string limitStr = std::to_string(limit);
    std::string offsetStr = std::to_string(offset);

    const char* paramValues[2] = { limitStr.c_str(), offsetStr.c_str() };

    PGresult* result = PQexecParams(
        guard.get(),
        "SELECT id, name, email FROM users ORDER BY id LIMIT $1 OFFSET $2",
        2, nullptr, paramValues, nullptr, nullptr, 0
    );

    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        std::string error = PQerrorMessage(guard.get());
        PQclear(result);
        throw std::runtime_error("Query failed: " + error);
    }

    std::vector<User> users;
    int rows = PQntuples(result);

    for (int row = 0; row < rows; ++row)
    {
        users.push_back(rowToUser(result, row));
    }

    PQclear(result);

    return users;
}

int UserRepository::count()
{
    PooledConnection guard(pool);

    PGresult* result = PQexec(guard.get(), "SELECT COUNT(*) FROM users");

    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        std::string error = PQerrorMessage(guard.get());
        PQclear(result);
        throw std::runtime_error("Query failed: " + error);
    }

    int total = std::stoi(PQgetvalue(result, 0, 0));

    PQclear(result);

    return total;
}

std::optional<User> UserRepository::getById(int id)
{
    PooledConnection guard(pool);

    std::string idStr = std::to_string(id);
    const char* paramValues[1] = { idStr.c_str() };

    PGresult* result = PQexecParams(
        guard.get(),
        "SELECT id, name, email, password_hash FROM users WHERE id = $1",
        1, nullptr, paramValues, nullptr, nullptr, 0
    );

    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        std::string error = PQerrorMessage(guard.get());
        PQclear(result);
        throw std::runtime_error("Query failed: " + error);
    }

    if (PQntuples(result) == 0)
    {
        PQclear(result);
        return std::nullopt;
    }

    User user = rowToUser(result, 0);

    PQclear(result);

    return user;
}

std::optional<User> UserRepository::getByEmail(const std::string& email)
{
    PooledConnection guard(pool);

    const char* paramValues[1] = { email.c_str() };

    PGresult* result = PQexecParams(
        guard.get(),
        "SELECT id, name, email, password_hash FROM users WHERE email = $1",
        1, nullptr, paramValues, nullptr, nullptr, 0
    );

    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        std::string error = PQerrorMessage(guard.get());
        PQclear(result);
        throw std::runtime_error("Query failed: " + error);
    }

    if (PQntuples(result) == 0)
    {
        PQclear(result);
        return std::nullopt;
    }

    User user = rowToUser(result, 0);

    PQclear(result);

    return user;
}

User UserRepository::create(
    const std::string& name,
    const std::string& email,
    const std::string& passwordHash)
{
    PooledConnection guard(pool);

    const char* paramValues[3] = { name.c_str(), email.c_str(), passwordHash.c_str() };

    PGresult* result = PQexecParams(
        guard.get(),
        "INSERT INTO users (name, email, password_hash) "
        "VALUES ($1, $2, $3) "
        "RETURNING id, name, email",
        3, nullptr, paramValues, nullptr, nullptr, 0
    );

    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        std::string error = PQerrorMessage(guard.get());
        PQclear(result);
        throw std::runtime_error("Insert failed: " + error);
    }

    User user = rowToUser(result, 0);
    user.passwordHash = passwordHash;

    PQclear(result);

    return user;
}

std::optional<User> UserRepository::update(
    int id,
    const std::string& name,
    const std::string& email)
{
    PooledConnection guard(pool);

    std::string idStr = std::to_string(id);
    const char* paramValues[3] = { name.c_str(), email.c_str(), idStr.c_str() };

    PGresult* result = PQexecParams(
        guard.get(),
        "UPDATE users "
        "SET name = $1, email = $2 "
        "WHERE id = $3 "
        "RETURNING id, name, email",
        3, nullptr, paramValues, nullptr, nullptr, 0
    );

    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        std::string error = PQerrorMessage(guard.get());
        PQclear(result);
        throw std::runtime_error("Update failed: " + error);
    }

    if (PQntuples(result) == 0)
    {
        PQclear(result);
        return std::nullopt;
    }

    User user = rowToUser(result, 0);

    PQclear(result);

    return user;
}

bool UserRepository::remove(int id)
{
    PooledConnection guard(pool);

    std::string idStr = std::to_string(id);
    const char* paramValues[1] = { idStr.c_str() };

    PGresult* result = PQexecParams(
        guard.get(),
        "DELETE FROM users WHERE id = $1",
        1, nullptr, paramValues, nullptr, nullptr, 0
    );

    if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::string error = PQerrorMessage(guard.get());
        PQclear(result);
        throw std::runtime_error("Delete failed: " + error);
    }

    std::string affected = PQcmdTuples(result);

    PQclear(result);

    return affected != "0" && !affected.empty();
}
