#include "ObjectsRef.h"

#include <fstream>
#include <iostream>
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
}

std::istream& operator>>(std::istream& in, ObjectsRef::Element& element)
{
    in >> element.label >> element.relative_dmd_path >> element.relative_texture_path;
    return in;
}
