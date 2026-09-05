#include <iostream>
#include "AuthController.h"

#include <utility>
#include <nlohmann/json.hpp>

#include "UserService.h"

using namespace Pistache;

AuthController::AuthController(AuthService& authService)
    : authService(authService)
{
}

void AuthController::registerRoutes(Rest::Router& router)
{
    Rest::Routes::Post(
        router,
        "/auth/register",
        [&](Rest::Request request, Http::ResponseWriter response)
        {
            registerUser(request, std::move(response));
            return Rest::Route::Result::Ok;
        }
    );

    Rest::Routes::Post(
        router,
        "/auth/login",
        [&](Rest::Request request, Http::ResponseWriter response)
        {
            login(request, std::move(response));
            return Rest::Route::Result::Ok;
        }
    );
}

namespace
{
    nlohmann::json authResultToJson(const AuthResult& result)
    {
        nlohmann::json userJson = result.user;

        return nlohmann::json{
            {"user", userJson},
            {"token", result.token}
        };
    }
}

void AuthController::registerUser(Rest::Request request, Http::ResponseWriter response)
{
    try
    {
        nlohmann::json body = nlohmann::json::parse(request.body());

        if (!body.contains("name") || !body.contains("email") || !body.contains("password"))
        {
            response.send(
                Http::Code::Bad_Request,
                R"({"error":"name, email and password are required"})",
                MIME(Application, Json)
            );

            return;
        }

        auto result = authService.registerUser(
            body.at("name").get<std::string>(),
            body.at("email").get<std::string>(),
            body.at("password").get<std::string>()
        );

        response.send(
            Http::Code::Created,
            authResultToJson(result).dump(),
            MIME(Application, Json)
        );
    }
    catch (const ValidationError& error)
    {
        response.send(
            Http::Code::Bad_Request,
            nlohmann::json{ {"error", error.what()} }.dump(),
            MIME(Application, Json)
        );
    }
    catch (const AuthError& error)
    {
        response.send(
            Http::Code::Conflict,
            nlohmann::json{ {"error", error.what()} }.dump(),
            MIME(Application, Json)
        );
    }
    catch (const nlohmann::json::exception&)
    {
        response.send(
            Http::Code::Bad_Request,
            R"({"error":"Invalid JSON"})",
            MIME(Application, Json)
        );
    }
    catch (const std::exception&)
    {
        response.send(
            Http::Code::Internal_Server_Error,
            R"({"error":"Internal server error"})",
            MIME(Application, Json)
        );
    }
}

void AuthController::login(Rest::Request request, Http::ResponseWriter response)
{
    try
    {
        nlohmann::json body = nlohmann::json::parse(request.body());

        if (!body.contains("email") || !body.contains("password"))
        {
            response.send(
                Http::Code::Bad_Request,
                R"({"error":"email and password are required"})",
                MIME(Application, Json)
            );

            return;
        }

        auto result = authService.login(
            body.at("email").get<std::string>(),
            body.at("password").get<std::string>()
        );

        response.send(
            Http::Code::Ok,
            authResultToJson(result).dump(),
            MIME(Application, Json)
        );
    }
    catch (const AuthError& error)
    {
        response.send(
            Http::Code::Unauthorized,
            nlohmann::json{ {"error", error.what()} }.dump(),
            MIME(Application, Json)
        );
    }
    catch (const nlohmann::json::exception&)
    {
        response.send(
            Http::Code::Bad_Request,
            R"({"error":"Invalid JSON"})",
            MIME(Application, Json)
        );
    }
    catch (const std::exception& error)
{
    std::cerr << "Unhandled error: " << error.what() << std::endl;

    response.send(
        Http::Code::Internal_Server_Error,
        R"({"error":"Internal server error"})",
        MIME(Application, Json)
    );
}
}
