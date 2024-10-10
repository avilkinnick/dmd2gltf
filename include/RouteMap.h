#ifndef ROUTE_MAP_H
#define ROUTE_MAP_H

#include <fstream>
#include <map>
#include <string>
#include <string_view>
#include <glm/ext/vector_float3.hpp>

#include "ObjectsRef.h"

class RouteMap
{
public:
    struct Transformation;

public:
    RouteMap(const std::string& full_dmd_route_path, ObjectsRef& objects_ref);

    void erase_redundant_elements(const ObjectsRef& objects_ref);

public:
    std::multimap<std::string, Transformation> elements {};

private:
    std::ifstream open_route_map(std::string_view full_route_map_path) const;
    void parse_route_map(std::ifstream&& route_map, ObjectsRef& objects_ref);
};

struct RouteMap::Transformation
{
    const glm::vec3 translation {};
    const glm::vec3 rotation {};
};

#endif // ROUTE_MAP_H
