#include "DmdRoute.h"

#include <exception>
#include <iostream>
#include <string_view>

#include "DmdMesh.h"
#include "ObjectsRef.h"
#include "RouteMap.h"

DmdRoute::DmdRoute(std::string_view full_dmd_route_path)
{
    ObjectsRef objects_ref { full_dmd_route_path.data() };
    RouteMap route_map { full_dmd_route_path.data(), objects_ref };

    for (const auto& relative_dmd_path : objects_ref.unique_relative_dmd_paths)
    {
        try
        {
            const DmdMesh dmd_mesh { std::string { full_dmd_route_path } + relative_dmd_path };
        }
        catch (const std::exception& exception)
        {
            std::cout << exception.what() << ' ' << relative_dmd_path << '\n';
        }
    }
}
