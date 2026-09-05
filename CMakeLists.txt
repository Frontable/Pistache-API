cmake_minimum_required(VERSION 3.16)
project(PistacheAPI CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(PISTACHE REQUIRED IMPORTED_TARGET libpistache)
pkg_check_modules(LIBPQ REQUIRED IMPORTED_TARGET libpq)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_library(pistache_api_core STATIC
    src/Config.cpp
    src/ConnectionPool.cpp
    src/UserRepository.cpp
    src/UserService.cpp
    src/AuthService.cpp
    src/PasswordHasher.cpp
    src/Jwt.cpp
    src/UserController.cpp
    src/AuthController.cpp
    src/AuthMiddleware.cpp
    src/HealthController.cpp
    src/StaticFileHandler.cpp
    src/LoggingHandler.cpp
)

target_include_directories(pistache_api_core PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(pistache_api_core PUBLIC
    PkgConfig::PISTACHE
    PkgConfig::LIBPQ
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
)

target_compile_definitions(pistache_api_core PUBLIC
    FRONTEND_DIR="${CMAKE_SOURCE_DIR}/frontend/"
)

add_executable(pistache_api src/main.cpp)
target_link_libraries(pistache_api PRIVATE pistache_api_core)

option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
