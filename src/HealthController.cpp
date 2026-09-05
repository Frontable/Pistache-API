#include "HealthController.h"

#include <utility>

using namespace Pistache;

void HealthController::registerRoutes(Rest::Router& router)
{
    Rest::Routes::Get(
        router,
        "/health",
        [&](Rest::Request request, Http::ResponseWriter response)
        {
            health(request, std::move(response));
            return Rest::Route::Result::Ok;
        }
    );
}

void HealthController::health(Rest::Request, Http::ResponseWriter response)
{
    response.send(
        Http::Code::Ok,
        R"({"status":"ok"})",
        MIME(Application, Json)
    );
}
