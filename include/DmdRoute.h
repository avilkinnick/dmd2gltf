#ifndef DMD_ROUTE_H
#define DMD_ROUTE_H

#include <string_view>

class DmdRoute
{
public:
    explicit DmdRoute(std::string_view full_dmd_route_path);
};

#endif // DMD_ROUTE_H
