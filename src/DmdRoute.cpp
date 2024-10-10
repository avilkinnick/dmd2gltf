#include "DmdRoute.h"

#include <exception>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <string_view>
#include <utility>

#include "DmdMesh.h"
#include "ObjectsRef.h"
#include "RouteMap.h"
#include "Vertex.h"
#include "tiny_gltf.h"

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

// TODO: refactor
DmdRoute::operator tinygltf::Model() const
{
    tinygltf::Model model {};

    model.buffers.resize(1);
    auto& buffer { model.buffers[0] };

    for (const auto& dmd_mesh : meshes)
    {
        for (const auto& vertex : dmd_mesh.vertices)
        {
            for (int i { 0 }; i < 3; ++i)
            {
                auto* ptr { (unsigned char*)&vertex.position[i] };
                for (int j { 0 }; j < sizeof(float); ++j)
                {
                    buffer.data.emplace_back(*(ptr + j));
                }
            }

            for (int i { 0 }; i < 2; ++i)
            {
                auto* ptr { (unsigned char*)&vertex.tex_coord[i] };
                for (int j { 0 }; j < sizeof(float); ++j)
                {
                    buffer.data.emplace_back(*(ptr + j));
                }
            }
        }

        for (int index : dmd_mesh.indices)
        {
            auto* ptr { (unsigned char*)&index };
            for (int i { 0 }; i < sizeof(unsigned int); ++i)
            {
                buffer.data.emplace_back(*(ptr + i));
            }
        }
    }

    model.bufferViews.resize(meshes.size() * 2);
    model.accessors.resize(meshes.size() * 3);

    const int dmd_mesh_count { (int)meshes.size() };
    int total_offset { 0 };
    for (int i { 0 }; i < dmd_mesh_count; ++i)
    {
        auto& vertices_buffer_view { model.bufferViews[i * 2] };
        vertices_buffer_view.buffer = 0;
        vertices_buffer_view.byteLength = meshes[i].vertices.size() * sizeof(Vertex);
        vertices_buffer_view.byteOffset = total_offset;
        vertices_buffer_view.byteStride = sizeof(Vertex);
        vertices_buffer_view.target = TINYGLTF_TARGET_ARRAY_BUFFER;
        total_offset += vertices_buffer_view.byteLength;

        auto& indices_buffer_view { model.bufferViews[i * 2 + 1] };
        indices_buffer_view.buffer = 0;
        indices_buffer_view.byteLength = meshes[i].indices.size() * sizeof(int);
        indices_buffer_view.byteOffset = total_offset;
        indices_buffer_view.target = TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER;
        total_offset += indices_buffer_view.byteLength;

        const auto& first_pos { meshes[i].vertices[0].position };
        float max_pos[3] {};
        float min_pos[3] {};
        max_pos[0] = min_pos[0] = first_pos.x;
        max_pos[1] = min_pos[1] = first_pos.y;
        max_pos[2] = min_pos[2] = first_pos.z;

        for (const auto& vertex : meshes[i].vertices)
        {
            for (int j { 0 }; j < 3; ++j)
            {
                if (vertex.position[j] > max_pos[j])
                {
                    max_pos[j] = vertex.position[j];
                }
                else if (vertex.position[j] < min_pos[j])
                {
                    min_pos[j] = vertex.position[j];
                }
            }
        }

        auto& position_accessor { model.accessors[i * 3] };
        position_accessor.bufferView = i * 2;
        position_accessor.byteOffset = 0;
        position_accessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
        position_accessor.type = TINYGLTF_TYPE_VEC3;
        position_accessor.count = meshes[i].vertices.size();
        for (int j { 0 }; j < 3; ++j)
        {
            position_accessor.maxValues.emplace_back(max_pos[j]);
            position_accessor.minValues.emplace_back(min_pos[j]);
        }

        auto& tex_coord_accessor { model.accessors[i * 3 + 1] };
        tex_coord_accessor.bufferView = i * 2;
        tex_coord_accessor.byteOffset = sizeof(glm::vec3);
        tex_coord_accessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
        tex_coord_accessor.type = TINYGLTF_TYPE_VEC2;
        tex_coord_accessor.count = meshes[i].vertices.size();

        auto& indices_accessor { model.accessors[i * 3 + 2] };
        indices_accessor.bufferView = i * 2 + 1;
        indices_accessor.byteOffset = 0;
        indices_accessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT;
        indices_accessor.type = TINYGLTF_TYPE_SCALAR;
        indices_accessor.count = meshes[i].indices.size();
    }

    model.samplers.resize(1);
    auto& sampler { model.samplers[0] };
    sampler.magFilter = TINYGLTF_TEXTURE_FILTER_LINEAR;
    sampler.minFilter = TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR;
    sampler.wrapS = TINYGLTF_TEXTURE_WRAP_REPEAT;
    sampler.wrapT = TINYGLTF_TEXTURE_WRAP_REPEAT;

    model.images.resize(relative_texture_paths.size());
    model.textures.resize(relative_texture_paths.size());
    model.materials.resize(relative_texture_paths.size());

    const int texture_count { (int)relative_texture_paths.size() };
    for (int i { 0 }; i < texture_count; ++i)
    {
        auto& image { model.images[i] };
        image.uri = relative_texture_paths[i];

        auto& texture { model.textures[i] };
        texture.sampler = 0;
        texture.source = i;

        auto& material { model.materials[i] };
        material.alphaMode = "MASK";
        material.pbrMetallicRoughness.baseColorTexture.index = i;
        material.pbrMetallicRoughness.baseColorTexture.texCoord = 0;
    }

    model.meshes.resize(models.size());
    for (const auto& [label, dmd_model] : models)
    {
        static int i { 0 };
        auto& mesh { model.meshes[i] };
        mesh.name = label;
        mesh.primitives.resize(1);
        auto& primitive { mesh.primitives[0] };
        primitive.attributes["POSITION"] = dmd_model.mesh_index * 3;
        primitive.attributes["TEXCOORD_0"] = dmd_model.mesh_index * 3 + 1;
        primitive.indices = dmd_model.mesh_index * 3 + 2;
        primitive.material = dmd_model.texture_index;
        primitive.mode = TINYGLTF_MODE_TRIANGLES;
        ++i;
    }

    model.nodes.resize(nodes.size());
    for (auto& [label, dmd_node] : nodes)
    {
        static int i { 0 };
        auto& node { model.nodes[i] };
        node.name = label;
        node.mesh = dmd_node.model_index;

        auto rotation { dmd_node.rotation };

        rotation.x = glm::radians(rotation.x);
        rotation.y = glm::radians(rotation.y);
        rotation.z = glm::radians(rotation.z);

        glm::mat4 transform(1.0f);
        transform = glm::rotate(transform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::translate(transform, dmd_node.translation);
        transform = glm::rotate(transform, -rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::rotate(transform, -rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, -rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));

        node.matrix.resize(16);
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                node.matrix[i * 4 + j] = transform[i][j];
            }
        }
        ++i;
    }

    model.scenes.resize(1);
    for (int i { 0 }; i < nodes.size(); ++i)
    {
        model.scenes[0].nodes.emplace_back(i);
    }

    return model;
};
