#ifndef DMD_ROUTE_H
#define DMD_ROUTE_H

#include <map>
#include <string>
#include <string_view>
#include <glm/ext/vector_float3.hpp>

#include "DmdMesh.h"

class DmdRoute
{
public:
    struct Model;
    struct Node;

public:
    explicit DmdRoute(std::string_view full_dmd_route_path);

private:
    std::vector<DmdMesh> meshes {};
    std::vector<std::string> relative_texture_paths {};
    std::map<std::string, Model> models {};
    std::multimap<std::string, Node> nodes {};
};

struct DmdRoute::Model
{
    int mesh_index {};
    int texture_index {};
    bool mipmap {};
    bool smooth {};
};

struct DmdRoute::Node
{
    int model_index {};
    glm::vec3 translation {};
    glm::vec3 rotation {};
};

#endif // DMD_ROUTE_H
