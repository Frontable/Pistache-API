#include "ConnectionPool.h"

#include <stdexcept>

ConnectionPool::ConnectionPool(const std::string& connectionString, std::size_t size)
{
    for (std::size_t i = 0; i < size; ++i)
    {
        PGconn* connection = PQconnectdb(connectionString.c_str());

        if (PQstatus(connection) != CONNECTION_OK)
        {
            std::string error = PQerrorMessage(connection);

            PQfinish(connection);

            while (!connections.empty())
            {
                PQfinish(connections.front());
                connections.pop();
            }

            throw std::runtime_error(
                "Database connection failed: " + error
            );
        }

        connections.push(connection);
    }
}

ConnectionPool::~ConnectionPool()
{
    std::lock_guard<std::mutex> lock(mutex);

    while (!connections.empty())
    {
        PQfinish(connections.front());
        connections.pop();
    }
}

PGconn* ConnectionPool::acquire()
{
    std::unique_lock<std::mutex> lock(mutex);

    conditionVariable.wait(lock, [this] { return !connections.empty(); });

    PGconn* connection = connections.front();
    connections.pop();

    return connection;
}

void ConnectionPool::release(PGconn* connection)
{
    {
        std::lock_guard<std::mutex> lock(mutex);
        connections.push(connection);
    }

    conditionVariable.notify_one();
}
