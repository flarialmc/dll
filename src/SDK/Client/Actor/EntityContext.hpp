#pragma once

class EntityId;

struct EntityIdTraits {
    using value_type = EntityId;

    using entity_type  = uint32_t;
    using version_type = uint16_t;

    static constexpr entity_type entity_mask  = 0x3FFFF;
    static constexpr entity_type version_mask = 0x3FFF;
};

template <>
class entt::entt_traits<EntityId> : public entt::basic_entt_traits<EntityIdTraits> {
public:
    static constexpr entity_type page_size = 2048;
};

struct IEntityComponent {};

template<std::derived_from<IEntityComponent> Type>
struct entt::component_traits<Type> {
    using type = Type;
    static constexpr bool in_place_delete = true;
    static constexpr std::size_t page_size = 128 * !std::is_empty_v<Type>;
};

template<typename Type>
struct entt::storage_type<Type, EntityId> {
    using type = basic_storage<Type, EntityId>;
};

class EntityId : public entt::entt_traits<EntityId> {
public:
    entity_type mRawId{};

    template <std::integral T>
    requires(!std::is_same_v<std::remove_cvref_t<T>, bool>)
    [[nodiscard]] constexpr EntityId(T rawId) : mRawId(static_cast<entity_type>(rawId)) {} // NOLINT

    [[nodiscard]] constexpr bool isNull() const { return *this == entt::null; }

    [[nodiscard]] constexpr operator entity_type() const { return mRawId; }

    constexpr bool operator==(const EntityId& other) const {
        return mRawId == other.mRawId;
    }
};

namespace V1_20_50 {
    class EntityRegistry : public std::enable_shared_from_this<EntityRegistry> {
    public:
        std::string name;
        entt::basic_registry<EntityId> ownedRegistry;
        uint32_t id;
    };

    class EntityContext {
    public:
        EntityRegistry *registry;
        entt::basic_registry<EntityId> *enttRegistry;
        EntityId entity;

        template<typename T>
        [[nodiscard]] T *tryGetComponent(EntityId id) {
            return this->enttRegistry->try_get<T>(id);
        }

        template<typename T>
        [[nodiscard]] T *tryGetComponent() {
            return this->enttRegistry->try_get<T>(this->entity);
        }

        template<typename T>
        [[nodiscard]] const T *tryGetComponent() const {
            return this->enttRegistry->try_get<T>(this->entity);
        }

        template<typename T>
        [[nodiscard]] bool hasComponent() const {
            return this->enttRegistry->all_of<T>(this->entity);
        }

        template<typename T>
        [[nodiscard]] bool hasComponent(EntityId id) const {
            return this->enttRegistry->all_of<T>(id);
        }

        template<typename T>
        void addComponent() {
            return this->enttRegistry->get_or_emplace<T>(this->entity);
        }

        template<typename T>
        void removeComponent() {
            this->enttRegistry->remove<T>(this->entity);
        }
    };
}

namespace V1_20_40 {
    struct EntityRegistryBase {
        entt::basic_registry<EntityId>& registry;
        uint32_t id;
    };

    struct EntityRegistry : EntityRegistryBase, std::enable_shared_from_this<EntityRegistry> {
        std::string name;
        entt::basic_registry<EntityId> ownedRegistry;
    };

    struct EntityContextBase {
        EntityRegistryBase& registry;
        EntityId entity;
    };

    struct EntityContext : EntityContextBase {
        entt::basic_registry<EntityId>& getEnttRegistry() const {
            return static_cast<EntityRegistry&>(this->registry).ownedRegistry;
        }
    };
}