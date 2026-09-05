find_package(Catch2 3 REQUIRED)

add_executable(unit_tests
    UserServiceTests.cpp
    AuthServiceTests.cpp
    JwtTests.cpp
    PasswordHasherTests.cpp
)

target_link_libraries(unit_tests PRIVATE
    pistache_api_core
    Catch2::Catch2WithMain
)

add_test(NAME unit_tests COMMAND unit_tests)
