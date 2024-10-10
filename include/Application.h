#ifndef APPLICATION_H
#define APPLICATION_H

#include <string_view>

#include "DmdRoute.h"

class Application
{
public:
    Application(int argc, char* argv[]);

    void run();

private:
    void copy_textures(DmdRoute& dmd_route);

private:
    std::string_view route_name {};
};

#endif // APPLICATION_H
