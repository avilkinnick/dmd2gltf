#include "ObjectsRef.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "string_functions.h"

ObjectsRef::ObjectsRef(const std::string& full_dmd_route_path)
{
    parse_objects_ref(open_objects_ref(full_dmd_route_path + "/objects.ref"));
    fill_unique_relative_paths();
    erase_invalid_paths(unique_relative_dmd_paths, full_dmd_route_path);
    erase_invalid_paths(unique_relative_texture_paths, full_dmd_route_path);
    erase_redundant_elements();
}

// TODO: split on several functions
void ObjectsRef::erase_redundant_elements()
{
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
        for (auto it { unique_relative_dmd_paths.begin() }; it != unique_relative_dmd_paths.end();)
        {
            bool found { false };
            for (const auto& element : elements)
            {
                if (*it == element.relative_dmd_path)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                it = unique_relative_dmd_paths.erase(it);
                erased = true;
            }
            else
            {
                ++it;
            }
        }

        for (auto it { unique_relative_texture_paths.begin() }; it != unique_relative_texture_paths.end();)
        {
            bool found { false };
            for (const auto& element : elements)
            {
                if (*it == element.relative_texture_path)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                it = unique_relative_texture_paths.erase(it);
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
    }
}

std::ifstream ObjectsRef::open_objects_ref(std::string_view full_objects_ref_path) const
{
    std::ifstream objects_ref { full_objects_ref_path.data() };
    if (!objects_ref)
    {
        throw std::runtime_error { "Failed to open objects.ref" };
    }

    return objects_ref;
}

void ObjectsRef::parse_objects_ref(std::ifstream&& objects_ref)
{
    bool mipmap { false };
    bool smooth { false };

    std::string line {};
    while (std::getline(objects_ref, line))
    {
        parse_line(line, mipmap, smooth);
    }

    if (elements.empty())
    {
        throw std::runtime_error { "Can not find elements in objects.ref" };
    }
}

void ObjectsRef::parse_line(std::string_view line, bool& mipmap, bool& smooth)
{
    if (line.empty())
    {
        return;
    }
    else if (line.front() == '[')
    {
        parse_property(line, mipmap, smooth);
    }
    else
    {
        std::string label {};
        std::string relative_dmd_path {};
        std::string relative_texture_path {};

        std::istringstream line_stream { line.data() };
        line_stream >> label >> relative_dmd_path >> relative_texture_path;

        if (!relative_texture_path.empty()
            && !is_slash(label.front())
            && is_slash(relative_dmd_path.front())
            && is_slash(relative_texture_path.front()))
        {
            std::replace(relative_dmd_path.begin(), relative_dmd_path.end(), '\\', '/');
            std::replace(relative_texture_path.begin(), relative_texture_path.end(), '\\', '/');

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

void ObjectsRef::parse_property(std::string_view line, bool& mipmap, bool& smooth) const noexcept
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

void ObjectsRef::fill_unique_relative_paths()
{
    for (const auto& element : elements)
    {
        unique_relative_dmd_paths.emplace(element.relative_dmd_path);
        unique_relative_texture_paths.emplace(element.relative_texture_path);
    }
}

void ObjectsRef::erase_invalid_paths(
    std::set<std::string_view>& unique_relative_paths,
    const std::string& full_dmd_route_path)
{
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
}
