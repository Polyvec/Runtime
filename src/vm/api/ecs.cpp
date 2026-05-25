#include "../../../include/ecs/world.hpp"
#include "../../../include/ecs/query.hpp"
#include "../../../include/math/vector2.hpp"
#include "../../../include/math/vector3.hpp"
#include "../../../include/math/vector4.hpp"
#include "../../../include/math/quaternion.hpp"
#include "../../../include/math/matrix3.hpp"
#include "../../../include/math/matrix4.hpp"
#include "../../../include/math/transform.hpp"
#include <sol/sol.hpp>
#include <unordered_map>
#include <functional>
#include <vector>
#include <string>
#include <new>

using namespace voxyl;
using namespace voxyl::ecs;
using namespace voxyl::math;

namespace {
    struct metatable {
        std::size_t size;
        bool object;
        std::function<sol::object(void*, sol::this_state)> to;
        std::function<void(void*, sol::object)> from;
    };

    std::unordered_map<uint32_t, metatable> registry;

    struct query {
        Query base;
        std::vector<uint32_t> tracks;

        query& with(uint32_t type) {
            base.with(type);
            tracks.push_back(type);
            return *this;
        }

        query& without(uint32_t type) {
            base.without(type);
            return *this;
        }

        query& any(sol::table table) {
            std::vector<uint32_t> types;
            table.for_each([&](sol::object key, sol::object value) {
                (void)key;
                uint32_t type = value.as<uint32_t>();
                types.push_back(type);
                tracks.push_back(type);
            });
            base.any(types);
            return *this;
        }

        bool has(uint32_t type) const {
            return base.has(type);
        }
    };
}

void register_ecs_bindings(lua_State* state) {
    sol::state_view view(state);
    sol::table ecs = view.create_named_table("ecs");

    ecs.new_usertype<query>("query",
        sol::no_constructor,
        "with", &query::with,
        "without", &query::without,
        "any", &query::any,
        "has", &query::has,
        "dispatch", [](query& self, sol::object command, sol::object layout, sol::object pipe, sol::table types) {
            (void)self; (void)command; (void)layout; (void)pipe; (void)types;
        }
    );

    ecs.new_usertype<World>("world",
        sol::constructors<World()>(),
        "component", [](sol::this_state state, World& world, const std::string& name, sol::object value) {
            (void)state;
            metatable meta;
            meta.object = false;
            if (value.is<Vector2>()) {
                meta.size = sizeof(Vector2);
                meta.to = [](void* pointer, sol::this_state ts) { return sol::make_object(ts, static_cast<Vector2*>(pointer)); };
                meta.from = [](void* pointer, sol::object object) { *static_cast<Vector2*>(pointer) = object.as<Vector2>(); };
            } else if (value.is<Vector3>()) {
                meta.size = sizeof(Vector3);
                meta.to = [](void* pointer, sol::this_state ts) { return sol::make_object(ts, static_cast<Vector3*>(pointer)); };
                meta.from = [](void* pointer, sol::object object) { *static_cast<Vector3*>(pointer) = object.as<Vector3>(); };
            } else if (value.is<Vector4>()) {
                meta.size = sizeof(Vector4);
                meta.to = [](void* pointer, sol::this_state ts) { return sol::make_object(ts, static_cast<Vector4*>(pointer)); };
                meta.from = [](void* pointer, sol::object object) { *static_cast<Vector4*>(pointer) = object.as<Vector4>(); };
            } else if (value.is<Quaternion>()) {
                meta.size = sizeof(Quaternion);
                meta.to = [](void* pointer, sol::this_state ts) { return sol::make_object(ts, static_cast<Quaternion*>(pointer)); };
                meta.from = [](void* pointer, sol::object object) { *static_cast<Quaternion*>(pointer) = object.as<Quaternion>(); };
            } else if (value.is<Matrix3>()) {
                meta.size = sizeof(Matrix3);
                meta.to = [](void* pointer, sol::this_state ts) { return sol::make_object(ts, static_cast<Matrix3*>(pointer)); };
                meta.from = [](void* pointer, sol::object object) { *static_cast<Matrix3*>(pointer) = object.as<Matrix3>(); };
            } else if (value.is<Matrix4>()) {
                meta.size = sizeof(Matrix4);
                meta.to = [](void* pointer, sol::this_state ts) { return sol::make_object(ts, static_cast<Matrix4*>(pointer)); };
                meta.from = [](void* pointer, sol::object object) { *static_cast<Matrix4*>(pointer) = object.as<Matrix4>(); };
            } else if (value.is<Transform>()) {
                meta.size = sizeof(Transform);
                meta.to = [](void* pointer, sol::this_state ts) { return sol::make_object(ts, static_cast<Transform*>(pointer)); };
                meta.from = [](void* pointer, sol::object object) { *static_cast<Transform*>(pointer) = object.as<Transform>(); };
            } else if (value.valid() && !value.is<sol::lua_nil_t>()) {
                meta.size = sizeof(sol::object);
                meta.object = true;
                meta.to = [](void* pointer, sol::this_state ts) { (void)ts; return *static_cast<sol::object*>(pointer); };
                meta.from = [](void* pointer, sol::object object) { ::new (pointer) sol::object(object); };
            } else {
                meta.size = 0;
                meta.to = [](void* pointer, sol::this_state ts) { (void)pointer; (void)ts; return sol::object(sol::lua_nil); };
                meta.from = [](void* pointer, sol::object object) { (void)pointer; (void)object; };
            }
            uint32_t type = world.component(name, meta.size);
            registry[type] = meta;
            return type;
        },
        "spawn", &World::spawn,
        "kill", &World::kill,
        "has", &World::has,
        "detach", &World::detach,
        "query", [](World& world) { return query{ world.query() }; },
        "batch", [](World& world, sol::function action) {
            (void)world;
            (void)action();
        },
        "attach", [](World& world, Entity entity, uint32_t type, sol::object value) {
            auto match = registry.find(type);
            if (match == registry.end()) return;
            const auto& meta = match->second;
            if (meta.size == 0) {
                world.attach(entity, type, nullptr);
                return;
            }
            std::vector<char> buffer(meta.size);
            meta.from(buffer.data(), value);
            world.attach(entity, type, buffer.data());
        },
        "get", [](sol::this_state state, World& world, Entity entity, uint32_t type) -> sol::object {
            void* pointer = world.get(entity, type);
            if (!pointer) return sol::lua_nil;
            auto match = registry.find(type);
            if (match == registry.end()) return sol::lua_nil;
            return match->second.to(pointer, state);
        },
        "execute", [](sol::this_state state, World& world, const query& filter, sol::function callback) {
            if (!callback.valid()) return;

            world.execute(filter.base, [state, callback, tracks = filter.tracks](std::size_t count, const Entity* entities, const std::vector<void*>& blocks) {
                (void)entities;
                sol::state_view context(state);
                std::vector<sol::object> inputs;
                inputs.reserve(blocks.size() + 1);
                inputs.push_back(sol::make_object(state, count));
                for (std::size_t step = 0; step < blocks.size(); ++step) {
                    if (step < tracks.size()) {
                        uint32_t type = tracks[step];
                        auto match = registry.find(type);
                        if (match != registry.end()) {
                            metatable meta = match->second;
                            sol::table column = context.create_table();
                            column["get"] = [block = blocks[step], meta](sol::this_state ts, sol::object self, std::size_t index) {
                                (void)self;
                                std::size_t offset = index - 1;
                                void* pointer = static_cast<char*>(block) + (offset * meta.size);
                                return meta.to(pointer, ts);
                            };
                            column["set"] = [block = blocks[step], meta](sol::object self, std::size_t index, sol::object value) {
                                (void)self;
                                std::size_t offset = index - 1;
                                void* pointer = static_cast<char*>(block) + (offset * meta.size);
                                meta.from(pointer, value);
                            };
                            inputs.push_back(column);
                            continue;
                        }
                    }
                    inputs.push_back(sol::lua_nil);
                }
                callback.call(sol::as_args(inputs));
            });
        }
    );
}