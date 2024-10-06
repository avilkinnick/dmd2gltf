#include <exception>
#include <iostream>

#include "Application.h"

int main(int argc, char* argv[])
{
    try
    {
        Application application { argc, argv };
        application.run();
    }
    catch (const std::exception& exception)
    {
        std::cerr << "\x1b[1;31mException: " << exception.what() << "\x1b[0m\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "\x1b[1;31mUnknown exception\x1b[0m\n";
        return 1;
    }

    return 0;
}
