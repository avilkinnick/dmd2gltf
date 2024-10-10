#include "DmdRoute.h"

#include <exception>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <string_view>
#include <utility>

#include "DmdMesh.h"
#include "ObjectsRef.h"
#include "RouteMap.h"

DmdRoute::DmdRoute(std::string_view full_dmd_route_path)
{
    ObjectsRef objects_ref { full_dmd_route_path.data() };
    RouteMap route_map { full_dmd_route_path.data(), objects_ref };

    std::map<std::string, DmdMesh> dmd_meshes {};

    for (const auto& relative_dmd_path : objects_ref.unique_relative_dmd_paths)
    {
        try
        {
            const DmdMesh dmd_mesh { std::string { full_dmd_route_path } + relative_dmd_path };
            dmd_meshes.emplace(relative_dmd_path, std::move(dmd_mesh));
        }
        catch (const std::exception& exception)
        {
            std::cout << exception.what() << ' ' << relative_dmd_path << '\n';
        }
    }

    for (auto it { objects_ref.unique_relative_dmd_paths.begin() }; it != objects_ref.unique_relative_dmd_paths.end();)
    {
        try
        {
            const DmdMesh dmd_mesh { std::string { full_dmd_route_path } + *it };
            ++it;
        }
        catch (const std::exception& exception)
        {
            std::cout << exception.what() << ' ' << *it << '\n';
            it = objects_ref.unique_relative_dmd_paths.erase(it);
        }
    }

    objects_ref.erase_redundant_elements();
    objects_ref.erase_redundant_paths();
    route_map.erase_redundant_elements(objects_ref);

    meshes.reserve(dmd_meshes.size());
    for (auto& [label, mesh] : dmd_meshes)
    {
        meshes.emplace_back(std::move(mesh));
    }

    relative_texture_paths.reserve(objects_ref.unique_relative_texture_paths.size());
    for (const auto& relative_path : objects_ref.unique_relative_texture_paths)
    {
        relative_texture_paths.emplace_back(relative_path);
    }

    for (const auto& element : objects_ref.elements)
    {
        models.emplace(
            std::move(element.first),
            Model {
                (int)std::distance(
                    objects_ref.unique_relative_dmd_paths.begin(),
                    objects_ref.unique_relative_dmd_paths.find(element.second.relative_dmd_path)
                ),
                (int)std::distance(
                    objects_ref.unique_relative_texture_paths.begin(),
                    objects_ref.unique_relative_texture_paths.find(element.second.relative_texture_path)
                ),
                element.second.mipmap,
                element.second.smooth
            }
        );
    }

    for (auto& element : route_map.elements)
    {
        nodes.emplace(
            element.first,
            Node {
                (int)std::distance(models.begin(), models.find(element.first)),
                std::move(element.second.translation),
                std::move(element.second.rotation)
            }
        );
    }
}
