#ifndef APPLICATION_H
#define APPLICATION_H

#include <string_view>

class Application
{
public:
    Application(int argc, char* argv[]);

    void run();

private:
    std::string_view route_name {};
};

#endif // APPLICATION_H
