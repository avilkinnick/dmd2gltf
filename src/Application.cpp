#include "Application.h"

#include <Magick++/Geometry.h>
#include <Magick++/Image.h>
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <ImageMagick-7/Magick++.h>

#include "cmake_defines.h"
#include "DmdRoute.h"

Application::Application(int argc, char* argv[])
{
    if (argc < 2)
    {
        throw std::runtime_error { "You need to pass route name as command-line argument" };
    }
    else if (argc > 2)
    {
        throw std::runtime_error { "Too many command-line arguments" };
    }
    else
    {
        route_name = argv[1];
    }
}

void Application::run()
{
    const std::string full_dmd_route_path { std::string { DMD_ROUTES_DIR } + '/' + route_name.data() };
    const std::string full_gltf_route_path { std::string { GLTF_ROUTES_DIR } + '/' + route_name.data() };

    if (!std::filesystem::exists(full_gltf_route_path) && !std::filesystem::create_directory(full_gltf_route_path))
    {
        throw std::runtime_error { "Failed to create gltf route directory" };
    }

    DmdRoute dmd_route { full_dmd_route_path };

    copy_textures(dmd_route);

    const auto gltf_route { static_cast<tinygltf::Model>(dmd_route) };

    tinygltf::TinyGLTF gltf {};
    gltf.WriteGltfSceneToFile(
        &gltf_route,
        std::string { GLTF_ROUTES_DIR } + '/' + route_name.data() + '/' + route_name.data() + ".gltf",
        false,
        false,
        true,
        false
    );
}

void Application::copy_textures(DmdRoute& dmd_route)
{
    for (auto& relative_path : dmd_route.relative_texture_paths)
    {
        Magick::Image image { std::string { DMD_ROUTES_DIR } + '/' + route_name.data() + relative_path };

        const auto width { image.columns() };
        const auto height { image.rows() };
        std::size_t new_width { 1 };
        std::size_t new_height { 1 };
        while (new_width < width)
        {
            new_width *= 2;
        }
        while (new_height < height)
        {
            new_height *= 2;
        }

        Magick::Geometry new_geometry {};
        new_geometry.aspect(true);
        new_geometry.width(new_width);
        new_geometry.height(new_height);

        relative_path.erase(0, 1);
        relative_path.erase(0, relative_path.find('/') + 1);
        relative_path = relative_path.substr(0, relative_path.find_last_of('.'));
        std::replace(relative_path.begin(), relative_path.end(), '/', '_');
        std::replace(relative_path.begin(), relative_path.end(), '\\', '_');
        relative_path += ".png";
        image.resize(new_geometry);

        image.write(std::string { GLTF_ROUTES_DIR } + '/' + route_name.data() + '/' + relative_path);
    }
}
