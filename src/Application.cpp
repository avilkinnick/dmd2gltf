#include "Application.h"

#include <stdexcept>

Application::Application(int argc, char* argv[])
{
    if (argc < 2)
    {
        throw std::runtime_error { "You need to pass route name as command-line argument" };
    }
    else if (argc > 2)
    {
        throw std::runtime_error { "Too many command-line arguments" };
    }
    else
    {
        route_name = argv[1];
    }
}

void Application::run()
{
}
