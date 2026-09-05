#include <catch2/catch_test_macros.hpp>

#include "Jwt.h"

TEST_CASE("Jwt::sign then Jwt::verify round-trips the payload", "[Jwt]")
{
    nlohmann::json payload = { {"sub", 42}, {"email", "alice@example.com"} };

    std::string token = Jwt::sign(payload, "secret", 3600);
    auto decoded = Jwt::verify(token, "secret");

    REQUIRE(decoded.has_value());
    REQUIRE((*decoded)["sub"] == 42);
    REQUIRE((*decoded)["email"] == "alice@example.com");
    REQUIRE(decoded->contains("iat"));
    REQUIRE(decoded->contains("exp"));
}

TEST_CASE("Jwt::verify rejects a token signed with a different secret", "[Jwt]")
{
    std::string token = Jwt::sign({ {"sub", 1} }, "secret-a", 3600);

    REQUIRE_FALSE(Jwt::verify(token, "secret-b").has_value());
}

TEST_CASE("Jwt::verify rejects a tampered payload", "[Jwt]")
{
    std::string token = Jwt::sign({ {"sub", 1} }, "secret", 3600);

    // Flip a character in the payload segment without re-signing.
    auto firstDot = token.find('.');
    auto secondDot = token.find('.', firstDot + 1);
    std::string tampered = token;
    tampered[firstDot + 1] = (tampered[firstDot + 1] == 'a') ? 'b' : 'a';

    REQUIRE_FALSE(Jwt::verify(tampered, "secret").has_value());
}

TEST_CASE("Jwt::verify rejects an expired token", "[Jwt]")
{
    std::string token = Jwt::sign({ {"sub", 1} }, "secret", -10); // expired 10s ago

    REQUIRE_FALSE(Jwt::verify(token, "secret").has_value());
}

TEST_CASE("Jwt::verify rejects a malformed token", "[Jwt]")
{
    REQUIRE_FALSE(Jwt::verify("not.a.validtoken!!!", "secret").has_value());
    REQUIRE_FALSE(Jwt::verify("missingdots", "secret").has_value());
}
