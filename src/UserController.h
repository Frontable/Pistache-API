#pragma once

#include <pistache/router.h>

#include "UserService.h"
#include "AuthService.h"

class UserController
{
public:
    UserController(UserService& service, AuthService& authService);

    void registerRoutes(Pistache::Rest::Router& router);

private:
    UserService& service;
    AuthService& authService;

    void getAll(
        Pistache::Rest::Request request,
        Pistache::Http::ResponseWriter response
    );

    void getById(
        Pistache::Rest::Request request,
        Pistache::Http::ResponseWriter response
    );

    void update(
        Pistache::Rest::Request request,
        Pistache::Http::ResponseWriter response
    );

    void remove(
        Pistache::Rest::Request request,
        Pistache::Http::ResponseWriter response
    );
};
