#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>

#include <libpq-fe.h>


class ConnectionPool
{
public:
    ConnectionPool(const std::string& connectionString, std::size_t size);

    ~ConnectionPool();

    ConnectionPool(const ConnectionPool&) = delete;
    ConnectionPool& operator=(const ConnectionPool&) = delete;

    // Blocks until a connection is available.
    PGconn* acquire();

    void release(PGconn* connection);

private:
    std::queue<PGconn*> connections;
    std::mutex mutex;
    std::condition_variable conditionVariable;
};

// RAII guard: borrows a connection on construction
class PooledConnection
{
public:
    explicit PooledConnection(ConnectionPool& pool)
        : pool(pool), connection(pool.acquire())
    {
    }

    ~PooledConnection()
    {
        pool.release(connection);
    }

    PooledConnection(const PooledConnection&) = delete;
    PooledConnection& operator=(const PooledConnection&) = delete;

    PGconn* get() const { return connection; }

private:
    ConnectionPool& pool;
    PGconn* connection;
};
