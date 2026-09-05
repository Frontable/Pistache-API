#include "StaticFileHandler.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

using namespace Pistache;


#ifndef FRONTEND_DIR
#define FRONTEND_DIR "../frontend/"
#endif

namespace
{
    const std::string& frontendDir()
    {
        static const std::string dir = [] {
            if (const char* fromEnv = std::getenv("FRONTEND_DIR"))
            {
                std::string value = fromEnv;

                if (!value.empty() && value.back() != '/')
                {
                    value.push_back('/');
                }

                return value;
            }

            return std::string(FRONTEND_DIR);
        }();

        return dir;
    }
}

void StaticFileHandler::handle(
    Http::Request request,
    Http::ResponseWriter response)
{
    std::string path = request.resource();

    if (path == "/")
    {
        path = "/index.html";
    }


    if (path.find("..") != std::string::npos)
    {
        response.send(
            Http::Code::Bad_Request,
            "Invalid path"
        );

        return;
    }

    std::string filePath = frontendDir() + path.substr(1);

    std::ifstream file(filePath);

    if (!file)
    {
        response.send(
            Http::Code::Not_Found,
            "File not found"
        );

        return;
    }

    std::stringstream buffer;

    buffer << file.rdbuf();

    std::string content = buffer.str();

    std::string mimeType;

    if (path.ends_with(".html"))
    {
        mimeType = "text/html";
    }
    else if (path.ends_with(".css"))
    {
        mimeType = "text/css";
    }
    else if (path.ends_with(".js"))
    {
        mimeType = "application/javascript";
    }
    else
    {
        mimeType = "text/plain";
    }

    response.send(
        Http::Code::Ok,
        content,
        Http::Mime::MediaType::fromString(mimeType)
    );
}
