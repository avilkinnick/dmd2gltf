#include "DmdRoute.h"

#include <string_view>

#include "ObjectsRef.h"
#include "RouteMap.h"

DmdRoute::DmdRoute(std::string_view full_dmd_route_path)
{
    ObjectsRef objects_ref { full_dmd_route_path.data() };
    RouteMap route_map { full_dmd_route_path.data(), objects_ref };
}
