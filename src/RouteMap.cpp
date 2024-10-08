#include "RouteMap.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "ObjectsRef.h"

RouteMap::RouteMap(const std::string& full_dmd_route_path, ObjectsRef& objects_ref)
{
    parse_route_map(open_route_map(full_dmd_route_path + "/route1.map"));
}

std::ifstream RouteMap::open_route_map(std::string_view full_route_map_path)
{
    std::ifstream route_map { full_route_map_path.data() };
    if (!route_map)
    {
        throw std::runtime_error { "Failed to open route1.map" };
    }

    return route_map;
}

void RouteMap::parse_route_map(std::ifstream&& route_map)
{
    std::string line {};
    while (std::getline(route_map, line))
    {
        std::string label {};
        glm::vec3 translation {};
        glm::vec3 rotation {};

        std::istringstream line_stream {};
        line_stream >> label;

        // TODO: finish
    }
}
