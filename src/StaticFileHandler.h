#pragma once

#include <pistache/http.h>

class StaticFileHandler
{
public:
    void handle(
        Pistache::Http::Request request,
        Pistache::Http::ResponseWriter response
    );
};
