#include "ObjectsRef.h"

#include <fstream>
#include <istream>
#include <stdexcept>
#include <string>
#include <string_view>

ObjectsRef::ObjectsRef(std::string_view full_dmd_route_path)
{
    std::ifstream file { std::string { full_dmd_route_path } + "/objects.ref" };
    if (!file)
    {
        throw std::runtime_error { "Failed to open objects.ref" };
    }

    bool mipmap { false };
    bool smooth { false };

    std::string line {};
    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        else if (line.front() == '[')
        {
            if (line == "[mipmap]")
            {
                mipmap = true;
            }
            else if (line == "[not_mipmap]")
            {
                mipmap = false;
            }
            else if (line == "[smooth]")
            {
                smooth = true;
            }
            else if (line == "[not_smooth]")
            {
            }
        }
    }
}

std::istream& operator>>(std::istream& in, ObjectsRef::Element& element)
{
    in >> element.label >> element.relative_dmd_path >> element.relative_texture_path;
    return in;
}
