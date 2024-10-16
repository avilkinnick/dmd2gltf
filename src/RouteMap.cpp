#include "RouteMap.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "ObjectsRef.h"

RouteMap::RouteMap(const std::string& full_dmd_route_path, ObjectsRef& objects_ref)
{
    parse_route_map(open_route_map(full_dmd_route_path + "/route1.map"), objects_ref);
    erase_redundant_elements(objects_ref);
}

void RouteMap::erase_redundant_elements(const ObjectsRef& objects_ref)
{
    for (auto it { elements.begin() }; it != elements.end();)
    {
        if (objects_ref.elements.find(it->first) == objects_ref.elements.end())
        {
            it = elements.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::ifstream RouteMap::open_route_map(std::string_view full_route_map_path) const
{
    std::ifstream route_map { full_route_map_path.data() };
    if (!route_map)
    {
        throw std::runtime_error { "Failed to open route1.map" };
    }

    return route_map;
}

// TODO: split on several functions
void RouteMap::parse_route_map(std::ifstream&& route_map, ObjectsRef& objects_ref)
{
    std::string line {};
    while (std::getline(route_map, line))
    {
        constexpr std::size_t min_line_length { 13 };

        if (line.length() < min_line_length
            || line.find_first_not_of(' ') == line.find_first_of(','))
        {
            continue;
        }

        line = line.substr(0, line.find_last_of(';'));
        std::replace(line.begin(), line.end(), ',', ' ');

        std::string label {};
        glm::vec3 translation {};
        glm::vec3 rotation {};

        std::istringstream line_stream { line };
        line_stream >> label;
        line_stream >> translation.x >> translation.y >> translation.z;
        line_stream >> rotation.x >> rotation.y >> rotation.z;

        if (line_stream.fail() || !line_stream.eof()
            || !std::isfinite(translation.x) || !std::isfinite(translation.y) || !std::isfinite(translation.z)
            || !std::isfinite(rotation.x) || !std::isfinite(rotation.y) || !std::isfinite(rotation.z))
        {
            std::cout << "Wrong line in route1.map:\n" << line << '\n';
            continue;
        }

        if (objects_ref.elements.find(label) != objects_ref.elements.end())
        {
            elements.emplace(label, Transformation { std::move(translation), std::move(rotation) });
        }
    }

    for (auto it { objects_ref.elements.begin() }; it != objects_ref.elements.end();)
    {
        if (elements.find(it->first) == elements.end())
        {
            it = objects_ref.elements.erase(it);
        }
        else
        {
            ++it;
        }
    }

    objects_ref.erase_redundant_paths();
}
