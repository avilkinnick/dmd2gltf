#include "ObjectsRef.h"

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "string_functions.h"

// TODO: split on several functions
ObjectsRef::ObjectsRef(const std::string& full_dmd_route_path)
{
    std::ifstream file { full_dmd_route_path + "/objects.ref" };
    if (!file)
    {
        throw std::runtime_error { "Failed to open objects.ref" };
    }

    bool mipmap { false };
    bool smooth { false };

    std::string line {};
    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        else if (line.front() == '[')
        {
            if (line == "[mipmap]")
            {
                mipmap = true;
            }
            else if (line == "[not_mipmap]")
            {
                mipmap = false;
            }
            else if (line == "[smooth]")
            {
                smooth = true;
            }
            else if (line == "[not_smooth]")
            {
                smooth = false;
            }
        }
        else
        {
            std::string label {};
            std::string relative_dmd_path {};
            std::string relative_texture_path {};

            std::istringstream line_stream { line };
            line_stream >> label >> relative_dmd_path >> relative_texture_path;

            if (!relative_texture_path.empty()
                && !is_slash(label.front())
                && is_slash(relative_dmd_path.front())
                && is_slash(relative_texture_path.front()))
            {
                elements.emplace(Element {
                    std::move(label),
                    std::move(relative_dmd_path),
                    std::move(relative_texture_path),
                    mipmap,
                    smooth
                });
            }
        }
    }

    std::set<std::string_view> unique_relative_dmd_paths {};
    std::set<std::string_view> unique_relative_texture_paths {};

    for (const auto& element : elements)
    {
        unique_relative_dmd_paths.emplace(element.relative_dmd_path);
        unique_relative_texture_paths.emplace(element.relative_texture_path);
    }

    const auto erase_missing1 { [&full_dmd_route_path](std::set<std::string_view>& unique_relative_paths) {
        for (auto it { unique_relative_paths.begin() }; it != unique_relative_paths.end();)
        {
            if (!std::ifstream { full_dmd_route_path + it->data() })
            {
                std::cout << "Failed to open " << it->data() << '\n';
                it = unique_relative_paths.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }};

    erase_missing1(unique_relative_dmd_paths);
    erase_missing1(unique_relative_texture_paths);

    while (true)
    {
        bool erased { false };
        for (auto it { elements.begin() }; it != elements.end();)
        {
            if (unique_relative_dmd_paths.find(it->relative_dmd_path) == unique_relative_dmd_paths.end()
                || unique_relative_texture_paths.find(it->relative_texture_path) == unique_relative_texture_paths.end())
            {
                it = elements.erase(it);
                erased = true;
            }
            else
            {
                ++it;
            }
        }

        if (!erased)
        {
            break;
        }

        erased = false;

        enum class SetType
        {
            dmd,
            texture
        };

        const auto erase_missing2 { [this, &erased](std::set<std::string_view>& unique_relative_paths, SetType type) {
            for (auto it { unique_relative_paths.begin() }; it != unique_relative_paths.end();)
            {
                bool found { false };
                for (const auto& element : elements)
                {
                    if (*it == ((type == SetType::dmd) ? element.relative_dmd_path : element.relative_texture_path))
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    it = unique_relative_paths.erase(it);
                    erased = true;
                }
                else
                {
                    ++it;
                }
            }
        }};

        erase_missing2(unique_relative_dmd_paths, SetType::dmd);
        erase_missing2(unique_relative_texture_paths, SetType::texture);
    }
}
