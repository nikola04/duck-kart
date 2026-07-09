#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace engine {
    using Entity = std::uint32_t;
    constexpr Entity NullEntity = 0;

    class World {
        public:
            Entity createEntity();
            void destroyEntity(Entity entity);

            template<typename T>
            T& add(Entity entity, T component);

            template<typename T>
            T* get(Entity entity);

            template<typename T>
            const T* get(Entity entity) const;

            template<typename T>
            bool has(Entity entity) const;

            template<typename T>
            void remove(Entity entity);

            const std::vector<Entity>& entities() const { return m_entities; }

        private:
            struct ComponentStorageBase {
                virtual ~ComponentStorageBase() = default;
                virtual void remove(Entity entity) = 0;
            };

            template<typename T>
            struct ComponentStorage final : ComponentStorageBase {
                std::unordered_map<Entity, T> components;

                void remove(Entity entity) override {
                    components.erase(entity);
                }
            };

            template<typename T>
            ComponentStorage<T>& storage();

            template<typename T>
            ComponentStorage<T>* storageIfExists();

            template<typename T>
            const ComponentStorage<T>* storageIfExists() const;

            Entity m_nextEntity = 1;

            std::vector<Entity> m_entities;
            std::unordered_map<std::type_index, std::unique_ptr<ComponentStorageBase>> m_componentStorages;
    };

    template<typename T>
    T& World::add(Entity entity, T component) {
        return storage<T>().components.insert_or_assign(entity, std::move(component)).first->second;
    }

    template<typename T>
    T* World::get(Entity entity) {
        auto* componentStorage = storageIfExists<T>();
        if (!componentStorage)
            return nullptr;

        auto& components = componentStorage->components;
        auto it = components.find(entity);

        if (it == components.end())
            return nullptr;

        return &it->second;
    }

    template<typename T>
    const T* World::get(Entity entity) const {
        const auto* componentStorage = storageIfExists<T>();
        if (!componentStorage)
            return nullptr;

        auto it = componentStorage->components.find(entity);
        if (it == componentStorage->components.end())
            return nullptr;

        return &it->second;
    }

    template<typename T>
    bool World::has(Entity entity) const {
        return get<T>(entity) != nullptr;
    }

    template<typename T>
    void World::remove(Entity entity) {
        auto* componentStorage = storageIfExists<T>();
        if (!componentStorage)
            return;

        componentStorage->components.erase(entity);
    }

    template<typename T>
    World::ComponentStorage<T>& World::storage() {
        const std::type_index type{typeid(T)};
        auto it = m_componentStorages.find(type);

        if (it == m_componentStorages.end()) {
            auto componentStorage = std::make_unique<ComponentStorage<T>>();
            auto* ptr = componentStorage.get();
            m_componentStorages.emplace(type, std::move(componentStorage));
            return *ptr;
        }

        return static_cast<ComponentStorage<T>&>(*it->second);
    }

    template<typename T>
    World::ComponentStorage<T>* World::storageIfExists() {
        const std::type_index type{typeid(T)};
        auto it = m_componentStorages.find(type);

        if (it == m_componentStorages.end())
            return nullptr;

        return static_cast<ComponentStorage<T>*>(it->second.get());
    }

    template<typename T>
    const World::ComponentStorage<T>* World::storageIfExists() const {
        const std::type_index type{typeid(T)};
        auto it = m_componentStorages.find(type);

        if (it == m_componentStorages.end())
            return nullptr;

        return static_cast<const ComponentStorage<T>*>(it->second.get());
    }
}
