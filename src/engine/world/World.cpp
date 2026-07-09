#include "World.hpp"

namespace engine {
    Entity World::createEntity() {
        Entity entity = m_nextEntity++;

        m_entities.push_back(entity);
        return entity;
    }

    void World::destroyEntity(Entity entity) {
        std::erase(m_entities, entity);

        for (auto& [type, storage] : m_componentStorages)
            storage->remove(entity);
    }
}
