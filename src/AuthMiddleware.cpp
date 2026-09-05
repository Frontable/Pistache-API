#include "AuthMiddleware.h"

using namespace Pistache;

namespace AuthMiddleware
{
    std::optional<int> requireAuth(
        const Http::Request& request,
        Http::ResponseWriter& response,
        const AuthService& authService)
    {
        auto authHeaderOpt = request.headers().tryGetRaw("Authorization");

        if (!authHeaderOpt.has_value())
        {
            response.send(
                Http::Code::Unauthorized,
                R"({"error":"Missing Authorization header"})",
                MIME(Application, Json)
            );

            return std::nullopt;
        }

        std::string value = authHeaderOpt->value();
        const std::string prefix = "Bearer ";

        if (value.rfind(prefix, 0) != 0)
        {
            response.send(
                Http::Code::Unauthorized,
                R"({"error":"Authorization header must be 'Bearer <token>'"})",
                MIME(Application, Json)
            );

            return std::nullopt;
        }

        std::string token = value.substr(prefix.size());
        auto userId = authService.verifyToken(token);

        if (!userId)
        {
            response.send(
                Http::Code::Unauthorized,
                R"({"error":"Invalid or expired token"})",
                MIME(Application, Json)
            );

            return std::nullopt;
        }

        return userId;
    }
}
