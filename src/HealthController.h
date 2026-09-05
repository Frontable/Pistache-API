#pragma once

#include <pistache/router.h>


class HealthController
{
public:
    void registerRoutes(Pistache::Rest::Router& router);

private:
    void health(
        Pistache::Rest::Request request,
        Pistache::Http::ResponseWriter response
    );
};
