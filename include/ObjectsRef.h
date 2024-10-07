#ifndef OBJECTS_REF_H
#define OBJECTS_REF_H

#include <fstream>
#include <set>
#include <string>
#include <string_view>

class ObjectsRef
{
public:
    class Element;

public:
    explicit ObjectsRef(const std::string& full_dmd_route_path);

private:
    std::ifstream open_objects_ref(const std::string& full_dmd_route_path);
    void parse_objects_ref(std::ifstream&& objects_ref);
    void parse_property(std::string_view line, bool& mipmap, bool& smooth) const noexcept;
    void parse_line(std::string_view line, bool mipmap, bool smooth);

private:
    std::set<Element> elements {};
    std::set<std::string_view> unique_relative_dmd_paths {};
    std::set<std::string_view> unique_relative_texture_paths {};
};

class ObjectsRef::Element
{
public:
    const std::string label {};
    const std::string relative_dmd_path {};
    const std::string relative_texture_path {};
    const bool mipmap {};
    const bool smooth {};

    bool operator<(const Element& rhs) const noexcept { return this->label < rhs.label; }
};

#endif // OBJECTS_REF_H
