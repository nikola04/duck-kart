#include "GLTFLoader.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace engine {
    static const unsigned char* accessorData(const tinygltf::Model& model, const tinygltf::Accessor& accessor) {
        const auto& bufferView = model.bufferViews[accessor.bufferView];
        const auto& buffer = model.buffers[bufferView.buffer];

        return buffer.data.data()
            + bufferView.byteOffset
            + accessor.byteOffset;
    }
    static std::vector<Vertex> loadVertices(
        const tinygltf::Model& model,
        const tinygltf::Primitive& primitive,
        const glm::mat4& transform,
        const glm::vec3& color
    ) {
        auto positionIt = primitive.attributes.find("POSITION");

        if (positionIt == primitive.attributes.end())
            throw std::runtime_error("GLB primitive has no POSITION attribute");

        const tinygltf::Accessor& positionAccessor = model.accessors[positionIt->second];

        if (positionAccessor.type != TINYGLTF_TYPE_VEC3 || positionAccessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
            throw std::runtime_error("POSITION must be vec3 float");

        const float* positions = reinterpret_cast<const float*>(accessorData(model, positionAccessor));

        std::vector<Vertex> vertices;
        vertices.reserve(positionAccessor.count);

        for (std::size_t i = 0; i < positionAccessor.count; ++i) {
            const float x = positions[i * 3 + 0], y = positions[i * 3 + 1], z = positions[i * 3 + 2];
            glm::vec4 worldPos = transform * glm::vec4{x, y, z, 1.0f};

            vertices.push_back(Vertex{
                .position = {worldPos.x, worldPos.y, worldPos.z},
                .color = color
            });
        }

        return vertices;
    }

    static std::vector<std::uint32_t> loadIndices(
        const tinygltf::Model& model,
        const tinygltf::Primitive& primitive
    ) {
        if (primitive.indices < 0)
            throw std::runtime_error("GLB primitive has no indices");

        const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];

        const unsigned char* indexData = accessorData(model, indexAccessor);

        std::vector<std::uint32_t> indices;
        indices.reserve(indexAccessor.count);

        for (std::size_t i = 0; i < indexAccessor.count; ++i) {
            std::uint32_t index = 0;

            switch (indexAccessor.componentType) {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    index = reinterpret_cast<const std::uint8_t*>(indexData)[i];
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    index = reinterpret_cast<const std::uint16_t*>(indexData)[i];
                    break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    index = reinterpret_cast<const std::uint32_t*>(indexData)[i];
                    break;
                default:
                    throw std::runtime_error("Unsupported index component type");
            }

            indices.push_back(index);
        }

        return indices;
    }

    static glm::vec3 primitiveColor(
        const tinygltf::Model& model,
        const tinygltf::Primitive& primitive
    ) {
        if (primitive.material < 0)
            return {1.0f, 0.7f, 0.2f};

        const auto& material = model.materials[primitive.material];
        const auto& baseColor = material.pbrMetallicRoughness.baseColorFactor;

        if (baseColor.size() < 3)
            return {1.0f, 0.7f, 0.2f};

        return {
            static_cast<float>(baseColor[0]),
            static_cast<float>(baseColor[1]),
            static_cast<float>(baseColor[2])
        };
    }

    static glm::mat4 nodeMatrix(const tinygltf::Node& node) {
        if (node.matrix.size() == 16)
            return glm::make_mat4(node.matrix.data());

        glm::mat4 result{1.0f};

        if (node.translation.size() == 3) {
            result = glm::translate(result, {
                static_cast<float>(node.translation[0]),
                static_cast<float>(node.translation[1]),
                static_cast<float>(node.translation[2])
            });
        }

        if (node.rotation.size() == 4) {
            glm::quat q{
                static_cast<float>(node.rotation[3]), // w
                static_cast<float>(node.rotation[0]), // x
                static_cast<float>(node.rotation[1]), // y
                static_cast<float>(node.rotation[2])  // z
            };

            result *= glm::mat4_cast(q);
        }

        if (node.scale.size() == 3) {
            result = glm::scale(result, {
                static_cast<float>(node.scale[0]),
                static_cast<float>(node.scale[1]),
                static_cast<float>(node.scale[2])
            });
        }

        return result;
    }

    static void processNode(
        const tinygltf::Model& model,
        int nodeIndex,
        const glm::mat4& parentTransform,
        std::vector<Vertex>& vertices,
        std::vector<std::uint32_t>& indices
    ) {
        const tinygltf::Node& node = model.nodes[nodeIndex];
        glm::mat4 globalTransform = parentTransform * nodeMatrix(node);

        if (node.mesh >= 0) {
            const tinygltf::Mesh& mesh = model.meshes[node.mesh];

            for (const auto& primitive : mesh.primitives) {
                std::uint32_t vertexOffset = static_cast<std::uint32_t>(vertices.size());

                const glm::vec3 color = primitiveColor(model, primitive);
                auto primitiveVertices = loadVertices(model, primitive, globalTransform, color);

                auto primitiveIndices = loadIndices(model, primitive);

                vertices.insert(
                    vertices.end(),
                    primitiveVertices.begin(),
                    primitiveVertices.end()
                );

                for (std::uint32_t index : primitiveIndices)
                    indices.push_back(vertexOffset + index);
            }
        }

        for (int child : node.children)
            processNode(model, child, globalTransform, vertices, indices);
    }

    Mesh GLTFLoader::loadMesh(const std::filesystem::path& path) {
        tinygltf::TinyGLTF loader;
        tinygltf::Model model;

        std::string error, warning;

        const bool loaded = loader.LoadBinaryFromFile(&model, &error, &warning, path.string());

        if (!warning.empty())
            std::cout << "Load mesh warning: " << warning << std::endl;

        if (!error.empty() || !loaded)
            throw std::runtime_error("Failed to load GLB file: " + path.string() + "\n" + error);

        if (model.meshes.empty())
            throw std::runtime_error("GLB file has no meshes: " + path.string());

        std::vector<Vertex> vertices;
        std::vector<std::uint32_t> indices;

        int sceneIndex = model.defaultScene >= 0 ? model.defaultScene : 0;
        const tinygltf::Scene& scene = model.scenes[sceneIndex];

        for (int nodeIndex : scene.nodes)
            processNode(model, nodeIndex, glm::mat4{1.0f}, vertices, indices);

        return Mesh{
            std::move(vertices),
            std::move(indices)
        };
    }
}
