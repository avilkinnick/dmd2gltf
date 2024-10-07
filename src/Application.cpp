#include "Application.h"

#include <stdexcept>
#include <string>

#include "cmake_defines.h"
#include "DmdRoute.h"

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
    const std::string full_dmd_route_path { std::string { DMD_ROUTES_DIR } + '/' + route_name.data() };
    const DmdRoute dmd_route { full_dmd_route_path };
}
