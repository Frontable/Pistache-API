#include <catch2/catch_test_macros.hpp>

#include "PasswordHasher.h"

TEST_CASE("PasswordHasher verifies a correct password", "[PasswordHasher]")
{
    std::string stored = PasswordHasher::hash("correct-horse-battery-staple");

    REQUIRE(PasswordHasher::verify("correct-horse-battery-staple", stored));
}

TEST_CASE("PasswordHasher rejects an incorrect password", "[PasswordHasher]")
{
    std::string stored = PasswordHasher::hash("correct-horse-battery-staple");

    REQUIRE_FALSE(PasswordHasher::verify("wrong-password", stored));
}

TEST_CASE("PasswordHasher never stores the password in plaintext", "[PasswordHasher]")
{
    std::string password = "correct-horse-battery-staple";
    std::string stored = PasswordHasher::hash(password);

    REQUIRE(stored.find(password) == std::string::npos);
}

TEST_CASE("PasswordHasher produces a different hash each time (random salt)", "[PasswordHasher]")
{
    std::string a = PasswordHasher::hash("same-password");
    std::string b = PasswordHasher::hash("same-password");

    REQUIRE(a != b);
    REQUIRE(PasswordHasher::verify("same-password", a));
    REQUIRE(PasswordHasher::verify("same-password", b));
}
