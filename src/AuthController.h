#pragma once

#include <pistache/router.h>

#include "AuthService.h"

class AuthController
{
public:
    explicit AuthController(AuthService& authService);

    void registerRoutes(Pistache::Rest::Router& router);

private:
    AuthService& authService;

    void registerUser(
        Pistache::Rest::Request request,
        Pistache::Http::ResponseWriter response
    );

    void login(
        Pistache::Rest::Request request,
        Pistache::Http::ResponseWriter response
    );
};
