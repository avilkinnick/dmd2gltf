#include "DmdRoute.h"

#include <string_view>

#include "ObjectsRef.h"

DmdRoute::DmdRoute(std::string_view full_dmd_route_path)
{
    ObjectsRef objects_ref { full_dmd_route_path.data() };
}
