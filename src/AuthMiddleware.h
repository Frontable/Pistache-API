#pragma once

#include <optional>

#include <pistache/http.h>

#include "AuthService.h"


namespace AuthMiddleware
{
    
    std::optional<int> requireAuth(
        const Pistache::Http::Request& request,
        Pistache::Http::ResponseWriter& response,
        const AuthService& authService
    );
}
