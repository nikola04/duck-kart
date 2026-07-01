#include "Mesh.hpp"
#include "Vertex.hpp"
#include <utility>
#include <vector>

namespace engine {
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<std::uint32_t> indices)
        : m_vertices(std::move(vertices)), m_indices(std::move(indices)) {}

    const std::vector<Vertex>& Mesh::vertices() const {
        return m_vertices;
    }

    const std::vector<std::uint32_t>& Mesh::indices() const {
        return m_indices;
    }
}
