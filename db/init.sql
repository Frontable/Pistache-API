-- Schema for the Pistache Users API.
-- Applied automatically by docker-compose (mounted into /docker-entrypoint-initdb.d/),
-- or run manually with: psql -U pistache_user -d pistache_api -f db/init.sql

CREATE TABLE IF NOT EXISTS users (
    id            SERIAL PRIMARY KEY,
    name          VARCHAR(255) NOT NULL,
    email         VARCHAR(255) NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    created_at    TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE INDEX IF NOT EXISTS idx_users_email ON users (email);
