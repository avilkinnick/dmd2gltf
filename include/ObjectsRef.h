#ifndef OBJECTS_REF_H
#define OBJECTS_REF_H

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <string_view>

class ObjectsRef
{
public:
    struct Element;

public:
    explicit ObjectsRef(const std::string& full_dmd_route_path);

    void erase_redundant_elements();

private:
    std::ifstream open_objects_ref(std::string_view full_objects_ref_path) const;
    void parse_objects_ref(std::ifstream&& objects_ref);
    void parse_line(std::string_view line, bool& mipmap, bool& smooth);
    void parse_property(std::string_view line, bool& mipmap, bool& smooth) const noexcept;

    void fill_unique_relative_paths();
    void erase_invalid_paths(
        std::set<std::string>& unique_relative_paths,
        const std::string& full_dmd_route_path);

private:
    std::map<std::string, Element> elements {};
    std::set<std::string> unique_relative_dmd_paths {};
    std::set<std::string> unique_relative_texture_paths {};

public:
    friend class RouteMap;
};

struct ObjectsRef::Element
{
    const std::string relative_dmd_path {};
    const std::string relative_texture_path {};
    const bool mipmap {};
    const bool smooth {};
};

#endif // OBJECTS_REF_H
