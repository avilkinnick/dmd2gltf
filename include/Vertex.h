#ifndef VERTEX_H
#define VERTEX_H

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

struct Vertex
{
    const glm::vec3 position {};
    const glm::vec2 tex_coord {};
};

#endif // VERTEX_H
