#include <iostream>

#include <pistache/endpoint.h>
#include <pistache/router.h>

#include "Config.h"
#include "ConnectionPool.h"
#include "UserRepository.h"
#include "UserService.h"
#include "AuthService.h"
#include "UserController.h"
#include "AuthController.h"
#include "HealthController.h"
#include "StaticFileHandler.h"
#include "LoggingHandler.h"

using namespace Pistache;

int main()
{
    Config config = Config::fromEnv();

    std::cout << "Connecting to database (" << config.dbHost << ":" << config.dbPort
              << "/" << config.dbName << ") with a pool of " << config.dbPoolSize
              << " connections...\n";

    ConnectionPool pool(config.connectionString(), config.dbPoolSize);

    UserRepository repository(pool);
    UserService service(repository);
    AuthService authService(repository, config.jwtSecret, config.jwtExpirySeconds);

    UserController userController(service, authService);
    AuthController authController(authService);
    HealthController healthController;
    StaticFileHandler staticFileHandler;

    Rest::Router router;

    userController.registerRoutes(router);
    authController.registerRoutes(router);
    healthController.registerRoutes(router);

    Rest::Routes::Get(
        router,
        "/",
        [&](Rest::Request request, Http::ResponseWriter response)
        {
            staticFileHandler.handle(request, std::move(response));
            return Rest::Route::Result::Ok;
        }
    );

    Rest::Routes::Get(
        router,
        "/style.css",
        [&](Rest::Request request, Http::ResponseWriter response)
        {
            staticFileHandler.handle(request, std::move(response));
            return Rest::Route::Result::Ok;
        }
    );

    Rest::Routes::Get(
        router,
        "/app.js",
        [&](Rest::Request request, Http::ResponseWriter response)
        {
            staticFileHandler.handle(request, std::move(response));
            return Rest::Route::Result::Ok;
        }
    );

    std::cout << "Creating server on port " << config.port << "...\n";

    Http::Endpoint server(
        Address(Ipv4::any(), Port(config.port))
    );

    auto opts = Http::Endpoint::options().threads(4);

    server.init(opts);

    
    server.setHandler(std::make_shared<LoggingHandler>(router.handler()));

    std::cout << "Serving on http://0.0.0.0:" << config.port << "\n";

    server.serve();

    std::cout << "serve() returned\n";
}
