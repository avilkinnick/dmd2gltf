#ifndef OBJECTS_REF_H
#define OBJECTS_REF_H

#include <iostream>
#include <set>
#include <string>
#include <string_view>

class ObjectsRef
{
public:
    class Element;

public:
    explicit ObjectsRef(std::string_view full_dmd_route_path);

private:
    std::set<Element> elements {};
};

class ObjectsRef::Element
{
    std::string label {};
    std::string relative_dmd_path {};
    std::string relative_texture_path {};
    bool mipmap {};
    bool smooth {};

    bool operator<(const Element& rhs) const noexcept { return this->label < rhs.label; }

    friend std::istream& operator>>(std::istream& in, Element& element);
};

#endif // OBJECTS_REF_H
