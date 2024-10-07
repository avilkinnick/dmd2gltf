#ifndef OBJECTS_REF_H
#define OBJECTS_REF_H

#include <set>
#include <string>

class ObjectsRef
{
public:
    class Element;

public:
    explicit ObjectsRef(const std::string& full_dmd_route_path);

private:
    std::set<Element> elements {};
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
