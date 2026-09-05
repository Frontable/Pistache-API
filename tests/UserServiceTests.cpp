#include <catch2/catch_test_macros.hpp>

#include "UserService.h"
#include "FakeUserRepository.h"

TEST_CASE("UserService returns paginated users in ascending id order", "[UserService]")
{
    FakeUserRepository repo;
    repo.create("Alice", "alice@example.com", "hash1");
    repo.create("Bob", "bob@example.com", "hash2");
    repo.create("Carol", "carol@example.com", "hash3");

    UserService service(repo);

    auto page1 = service.getAll(1, 2);
    REQUIRE(page1.users.size() == 2);
    REQUIRE(page1.users[0].name == "Alice");
    REQUIRE(page1.users[1].name == "Bob");
    REQUIRE(page1.total == 3);

    auto page2 = service.getAll(2, 2);
    REQUIRE(page2.users.size() == 1);
    REQUIRE(page2.users[0].name == "Carol");
}

TEST_CASE("UserService clamps out-of-range pagination params", "[UserService]")
{
    FakeUserRepository repo;
    repo.create("Alice", "alice@example.com", "hash1");

    UserService service(repo);

    // page < 1 is treated as page 1
    auto result = service.getAll(0, 1000);
    REQUIRE(result.page == 1);
    REQUIRE(result.limit == 100);
    REQUIRE(result.users.size() == 1);
}

TEST_CASE("UserService::getById returns nullopt for a missing user", "[UserService]")
{
    FakeUserRepository repo;
    UserService service(repo);

    REQUIRE_FALSE(service.getById(999).has_value());
}

TEST_CASE("UserService::update rejects an invalid email", "[UserService][validation]")
{
    FakeUserRepository repo;
    int id = repo.create("Alice", "alice@example.com", "hash1").id;

    UserService service(repo);

    REQUIRE_THROWS_AS(service.update(id, "Alice", "not-an-email"), ValidationError);
}

TEST_CASE("UserService::update rejects an empty name", "[UserService][validation]")
{
    FakeUserRepository repo;
    int id = repo.create("Alice", "alice@example.com", "hash1").id;

    UserService service(repo);

    REQUIRE_THROWS_AS(service.update(id, "", "alice@example.com"), ValidationError);
}

TEST_CASE("UserService::update changes name and email for a valid user", "[UserService]")
{
    FakeUserRepository repo;
    int id = repo.create("Alice", "alice@example.com", "hash1").id;

    UserService service(repo);

    auto updated = service.update(id, "Alice Smith", "alice.smith@example.com");

    REQUIRE(updated.has_value());
    REQUIRE(updated->name == "Alice Smith");
    REQUIRE(updated->email == "alice.smith@example.com");
}

TEST_CASE("UserService::update returns nullopt for a missing user", "[UserService]")
{
    FakeUserRepository repo;
    UserService service(repo);

    REQUIRE_FALSE(service.update(999, "Someone", "someone@example.com").has_value());
}

TEST_CASE("UserService::remove reports whether a user existed", "[UserService]")
{
    FakeUserRepository repo;
    int id = repo.create("Alice", "alice@example.com", "hash1").id;

    UserService service(repo);

    REQUIRE(service.remove(id));
    REQUIRE_FALSE(service.remove(id)); // already gone
}
