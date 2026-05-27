#include "../include/vm/api/ecs.hpp"
#include "../include/core/ecs/world.hpp"
#include "../include/core/ecs/query.hpp"
#include "../include/core/types/vector2.hpp"
#include "../include/core/types/vector3.hpp"
#include "../include/core/types/vector4.hpp"
#include "../include/core/types/quaternion.hpp"
#include "../include/core/types/matrix3.hpp"
#include "../include/core/types/matrix4.hpp"
#include "../include/core/types/transform.hpp"
#include <sol/sol.hpp>
#include <unordered_map>
#include <functional>
#include <vector>
#include <string>
#include <memory>

using namespace voxyl::ecs;
using namespace voxyl::math;

namespace {
    struct detail {
        std::size_t size;
        bool object;
        std::function<sol::object(void*, sol::this_state)> reader;
        std::function<void(void*, sol::object)> writer;
    };

    struct column {
        void* block;
        std::size_t size;
        std::function<sol::object(void*, sol::this_state)> reader;
        std::function<void(void*, sol::object)> writer;

        sol::object get(sol::this_state context, std::size_t position) {
            void* pointer = static_cast<char*>(block) + ((position - 1) * size);
            return reader(pointer, context);
        }

        void set(std::size_t position, sol::object value) {
            void* pointer = static_cast<char*>(block) + ((position - 1) * size);
            writer(pointer, value);
        }
    };

    struct query {
        Query base;

        query& with(uint32_t type) {
            base.with(type);
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
                types.push_back(value.as<uint32_t>());
            });
            base.any(types);
            return *this;
        }

        bool has(uint32_t type) const {
            return base.has(type);
        }
    };
}

void ecs(lua_State* state) {
    sol::state_view lua(state);
    sol::table world = lua.create_named_table("world");
    sol::table hidden = lua.create_table();

    auto store = std::make_shared<std::unordered_map<uint32_t, detail>>();

    hidden.new_usertype<column>("column",
        sol::no_constructor,
        "get", &column::get,
        "set", &column::set
    );

    hidden.new_usertype<query>("query",
        sol::no_constructor,
        "with", &query::with,
        "without", &query::without,
        "any", &query::any,
        "has", &query::has
    );

    hidden.new_usertype<World>("World",
        sol::no_constructor,
        "component", [store](sol::this_state state, World& instance, const std::string& name, sol::object value) {
            (void)state;
            detail item;
            item.object = false;
            if (value.is<Vector2>()) {
                item.size = sizeof(Vector2);
                item.reader = [](void* pointer, sol::this_state context) { return sol::make_object(context, static_cast<Vector2*>(pointer)); };
                item.writer = [](void* pointer, sol::object object) { *static_cast<Vector2*>(pointer) = object.as<Vector2>(); };
            } else if (value.is<Vector3>()) {
                item.size = sizeof(Vector3);
                item.reader = [](void* pointer, sol::this_state context) { return sol::make_object(context, static_cast<Vector3*>(pointer)); };
                item.writer = [](void* pointer, sol::object object) { *static_cast<Vector3*>(pointer) = object.as<Vector3>(); };
            } else if (value.is<Vector4>()) {
                item.size = sizeof(Vector4);
                item.reader = [](void* pointer, sol::this_state context) { return sol::make_object(context, static_cast<Vector4*>(pointer)); };
                item.writer = [](void* pointer, sol::object object) { *static_cast<Vector4*>(pointer) = object.as<Vector4>(); };
            } else if (value.is<Quaternion>()) {
                item.size = sizeof(Quaternion);
                item.reader = [](void* pointer, sol::this_state context) { return sol::make_object(context, static_cast<Quaternion*>(pointer)); };
                item.writer = [](void* pointer, sol::object object) { *static_cast<Quaternion*>(pointer) = object.as<Quaternion>(); };
            } else if (value.is<Matrix3>()) {
                item.size = sizeof(Matrix3);
                item.reader = [](void* pointer, sol::this_state context) { return sol::make_object(context, static_cast<Matrix3*>(pointer)); };
                item.writer = [](void* pointer, sol::object object) { *static_cast<Matrix3*>(pointer) = object.as<Matrix3>(); };
            } else if (value.is<Matrix4>()) {
                item.size = sizeof(Matrix4);
                item.reader = [](void* pointer, sol::this_state context) { return sol::make_object(context, static_cast<Matrix4*>(pointer)); };
                item.writer = [](void* pointer, sol::object object) { *static_cast<Matrix4*>(pointer) = object.as<Matrix4>(); };
            } else if (value.is<Transform>()) {
                item.size = sizeof(Transform);
                item.reader = [](void* pointer, sol::this_state context) { return sol::make_object(context, static_cast<Transform*>(pointer)); };
                item.writer = [](void* pointer, sol::object object) { *static_cast<Transform*>(pointer) = object.as<Transform>(); };
            } else if (value.valid() && !value.is<sol::lua_nil_t>()) {
                item.size = sizeof(int);
                item.object = true;
                item.reader = [](void* pointer, sol::this_state context) {
                    int reference = *static_cast<int*>(pointer);
                    lua_rawgeti(context, LUA_REGISTRYINDEX, reference);
                    sol::object object(context, -1);
                    lua_pop(context, 1);
                    return object;
                };
                item.writer = [](void* pointer, sol::object object) {
                    int* cell = static_cast<int*>(pointer);
                    lua_State* current = object.lua_state();
                    if (*cell > 0) {
                        luaL_unref(current, LUA_REGISTRYINDEX, *cell);
                    }
                    *cell = luaL_ref(current, LUA_REGISTRYINDEX);
                };
            } else {
                item.size = 0;
                item.reader = [](void* pointer, sol::this_state context) { (void)pointer; (void)context; return sol::object(sol::lua_nil); };
                item.writer = [](void* pointer, sol::object object) { (void)pointer; (void)object; };
            }
            uint32_t type = instance.component(name, item.size);
            (*store)[type] = item;
            return type;
        },
        "spawn", &World::spawn,
        "kill", &World::kill,
        "has", &World::has,
        "detach", &World::detach,
        "query", [](World& instance) { return query{ instance.query() }; },
        "batch", [](World& instance, sol::function action) {
            instance.batch([action]() { (void)action(); });
        },
        "attach", [store](World& instance, Entity entity, uint32_t type, sol::object value) {
            auto match = store->find(type);
            if (match == store->end()) return;
            const auto& item = match->second;
            if (item.size == 0) {
                instance.attach(entity, type, nullptr);
                return;
            }
            std::vector<char> memory(item.size, 0);
            item.writer(memory.data(), value);
            instance.attach(entity, type, memory.data());
        },
        "get", [store](sol::this_state state, World& instance, Entity entity, uint32_t type) -> sol::object {
            void* pointer = instance.get(entity, type);
            if (!pointer) return sol::lua_nil;
            auto match = store->find(type);
            if (match == store->end()) return sol::lua_nil;
            return match->second.reader(pointer, state);
        },
        "execute", [store](sol::this_state state, World& instance, const query& search, sol::function callback) {
            if (!callback.valid()) return;
            instance.execute(search.base, [state, callback, store, order = search.base.tracked()](std::size_t count, const Entity* entities, const std::vector<void*>& blocks) {
                (void)entities;
                std::vector<sol::object> arguments;
                arguments.reserve(blocks.size() + 1);
                arguments.push_back(sol::make_object(state, count));
                for (std::size_t index = 0; index < blocks.size(); ++index) {
                    if (index < order.size()) {
                        uint32_t type = order[index];
                        auto match = store->find(type);
                        if (match != store->end()) {
                            const auto& item = match->second;
                            arguments.push_back(sol::make_object(state, column{ blocks[index], item.size, item.reader, item.writer }));
                            continue;
                        }
                    }
                    arguments.push_back(sol::lua_nil);
                }
                (void)callback.call(sol::as_args(arguments));
            });
        }
    );

    world["new"] = []() {
        return World();
    };
}