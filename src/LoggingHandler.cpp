#include "LoggingHandler.h"

#include <chrono>
#include <iostream>
#include <iomanip>

using namespace Pistache;

LoggingHandler::LoggingHandler(std::shared_ptr<Http::Handler> next)
    : next(std::move(next))
{
}

void LoggingHandler::onRequest(const Http::Request& request, Http::ResponseWriter response)
{
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    
    std::cout << "time=" << std::put_time(std::gmtime(&now), "%Y-%m-%dT%H:%M:%SZ")
               << " method=" << request.method()
               << " path=" << request.resource()
               << std::endl;

    next->onRequest(request, std::move(response));
}
