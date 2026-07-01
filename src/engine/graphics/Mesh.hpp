#pragma once

#include "Vertex.hpp"
#include <cstdint>
#include <vector>

namespace engine {
    class Mesh {
        public:
            Mesh(std::vector<Vertex> vertices, std::vector<std::uint32_t> indices);

            const std::vector<Vertex>& vertices() const;
            const std::vector<std::uint32_t>& indices() const;

        private:
            std::vector<Vertex> m_vertices;
            std::vector<std::uint32_t> m_indices;
    };
}
