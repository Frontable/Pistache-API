#include "UserController.h"
#include <iostream>
#include <utility>
#include <nlohmann/json.hpp>

#include "AuthMiddleware.h"

using namespace Pistache;

UserController::UserController(UserService& service, AuthService& authService)
    : service(service), authService(authService)
{
}

void UserController::registerRoutes(Rest::Router& router)
{
    Rest::Routes::Get(
        router,
        "/users",
        [&](Rest::Request request, Http::ResponseWriter response)
        {
            getAll(request, std::move(response));
            return Rest::Route::Result::Ok;
        }
    );

    Rest::Routes::Get(
        router,
        "/users/:id",
        [&](Rest::Request request, Http::ResponseWriter response)
        {
            getById(request, std::move(response));
            return Rest::Route::Result::Ok;
        }
    );

    Rest::Routes::Put(
        router,
        "/users/:id",
        [&](Rest::Request request, Http::ResponseWriter response)
        {
            update(request, std::move(response));
            return Rest::Route::Result::Ok;
        }
    );

    Rest::Routes::Delete(
        router,
        "/users/:id",
        [&](Rest::Request request, Http::ResponseWriter response)
        {
            remove(request, std::move(response));
            return Rest::Route::Result::Ok;
        }
    );
}

void UserController::getAll(Rest::Request request, Http::ResponseWriter response)
{
    try
    {
        int page = 1;
        int limit = 20;

        if (request.query().has("page"))
        {
            page = std::stoi(request.query().get("page").value());
        }

        if (request.query().has("limit"))
        {
            limit = std::stoi(request.query().get("limit").value());
        }

        PagedUsers result = service.getAll(page, limit);

        nlohmann::json responseJson = {
            {"data", result.users},
            {"page", result.page},
            {"limit", result.limit},
            {"total", result.total}
        };

        response.send(Http::Code::Ok, responseJson.dump(), MIME(Application, Json));
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

void UserController::getById(Rest::Request request, Http::ResponseWriter response)
{
    try
    {
        int id = request.param(":id").as<int>();

        auto user = service.getById(id);

        if (!user)
        {
            response.send(
                Http::Code::Not_Found,
                R"({"error":"User not found"})",
                MIME(Application, Json)
            );

            return;
        }

        nlohmann::json responseJson = *user;

        response.send(Http::Code::Ok, responseJson.dump(), MIME(Application, Json));
    }
    catch (const std::exception&)
    {
        response.send(
            Http::Code::Internal_Server_Error,
            R"({"error":"Database error"})",
            MIME(Application, Json)
        );
    }
}

void UserController::update(Rest::Request request, Http::ResponseWriter response)
{
    if (!AuthMiddleware::requireAuth(request, response, authService))
    {
        return; 
    }

    try
    {
        int id = request.param(":id").as<int>();

        nlohmann::json body = nlohmann::json::parse(request.body());

        if (!body.contains("name") || !body.contains("email"))
        {
            response.send(
                Http::Code::Bad_Request,
                R"({"error":"name and email are required"})",
                MIME(Application, Json)
            );

            return;
        }

        auto user = service.update(
            id,
            body.at("name").get<std::string>(),
            body.at("email").get<std::string>()
        );

        if (!user)
        {
            response.send(
                Http::Code::Not_Found,
                R"({"error":"User not found"})",
                MIME(Application, Json)
            );

            return;
        }

        nlohmann::json responseJson = *user;

        response.send(Http::Code::Ok, responseJson.dump(), MIME(Application, Json));
    }
    catch (const ValidationError& error)
    {
        response.send(
            Http::Code::Bad_Request,
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
            R"({"error":"Database error"})",
            MIME(Application, Json)
        );
    }
}

void UserController::remove(Rest::Request request, Http::ResponseWriter response)
{
    if (!AuthMiddleware::requireAuth(request, response, authService))
    {
        return; // requireAuth already sent a 401
    }

    try
    {
        int id = request.param(":id").as<int>();

        bool deleted = service.remove(id);

        if (!deleted)
        {
            response.send(
                Http::Code::Not_Found,
                R"({"error":"User not found"})",
                MIME(Application, Json)
            );

            return;
        }

        response.send(Http::Code::No_Content, "");
    }
    catch (const std::exception&)
    {
        response.send(
            Http::Code::Internal_Server_Error,
            R"({"error":"Database error"})",
            MIME(Application, Json)
        );
    }
}
