#include <catch2/catch_test_macros.hpp>

#include "AuthService.h"
#include "UserService.h"
#include "FakeUserRepository.h"

TEST_CASE("AuthService registers a user and issues a usable token", "[AuthService]")
{
    FakeUserRepository repo;
    AuthService auth(repo, "test-secret", 3600);

    auto result = auth.registerUser("Alice", "alice@example.com", "hunter22");

    REQUIRE(result.user.name == "Alice");
    REQUIRE(result.user.email == "alice@example.com");
    REQUIRE_FALSE(result.token.empty());

    auto userId = auth.verifyToken(result.token);
    REQUIRE(userId.has_value());
    REQUIRE(*userId == result.user.id);
}

TEST_CASE("AuthService rejects registering a duplicate email", "[AuthService]")
{
    FakeUserRepository repo;
    AuthService auth(repo, "test-secret", 3600);

    auth.registerUser("Alice", "alice@example.com", "hunter22");

    REQUIRE_THROWS_AS(
        auth.registerUser("Alice Two", "alice@example.com", "anotherpass"),
        AuthError
    );
}

TEST_CASE("AuthService rejects a too-short password", "[AuthService][validation]")
{
    FakeUserRepository repo;
    AuthService auth(repo, "test-secret", 3600);

    REQUIRE_THROWS_AS(
        auth.registerUser("Alice", "alice@example.com", "short"),
        ValidationError
    );
}

TEST_CASE("AuthService::login succeeds with correct credentials", "[AuthService]")
{
    FakeUserRepository repo;
    AuthService auth(repo, "test-secret", 3600);

    auth.registerUser("Alice", "alice@example.com", "hunter22");

    auto result = auth.login("alice@example.com", "hunter22");
    REQUIRE(result.user.email == "alice@example.com");
    REQUIRE_FALSE(result.token.empty());
}

TEST_CASE("AuthService::login fails with the wrong password", "[AuthService]")
{
    FakeUserRepository repo;
    AuthService auth(repo, "test-secret", 3600);

    auth.registerUser("Alice", "alice@example.com", "hunter22");

    REQUIRE_THROWS_AS(auth.login("alice@example.com", "wrongpassword"), AuthError);
}

TEST_CASE("AuthService::login fails for an unknown email", "[AuthService]")
{
    FakeUserRepository repo;
    AuthService auth(repo, "test-secret", 3600);

    REQUIRE_THROWS_AS(auth.login("nobody@example.com", "whatever1"), AuthError);
}

TEST_CASE("AuthService::verifyToken rejects a token signed with a different secret", "[AuthService]")
{
    FakeUserRepository repo;
    AuthService auth(repo, "test-secret", 3600);
    AuthService otherAuth(repo, "different-secret", 3600);

    auto result = auth.registerUser("Alice", "alice@example.com", "hunter22");

    REQUIRE_FALSE(otherAuth.verifyToken(result.token).has_value());
}

TEST_CASE("AuthService::verifyToken rejects an already-expired token", "[AuthService]")
{
    FakeUserRepository repo;
    AuthService auth(repo, "test-secret", -1); // expires immediately

    auto result = auth.registerUser("Alice", "alice@example.com", "hunter22");

    REQUIRE_FALSE(auth.verifyToken(result.token).has_value());
}
