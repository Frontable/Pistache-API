#pragma once

#include <memory>

#include <pistache/http.h>


class LoggingHandler : public Pistache::Http::Handler
{
public:
    HTTP_PROTOTYPE(LoggingHandler)

    explicit LoggingHandler(std::shared_ptr<Pistache::Http::Handler> next);

    void onRequest(
        const Pistache::Http::Request& request,
        Pistache::Http::ResponseWriter response
    ) override;

private:
    std::shared_ptr<Pistache::Http::Handler> next;
};
