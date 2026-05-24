#include "../../../include/ecs/world.hpp"
#include "../../../include/ecs/query.hpp"
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <LuaBridge/Vector.h>
#include <mutex>
#include <memory>

using namespace voxyl;
using namespace voxyl::ecs;

void register_ecs_bindings(lua_State* state) {
    luabridge::getGlobalNamespace(state)
        .beginNamespace("ecs")

            .beginClass<Query>("query")
                .addFunction("with", &Query::with)
                .addFunction("without", &Query::without)
                .addFunction("any", &Query::any)
                .addFunction("has", &Query::has)
                .addFunction("run", [](const Query& query, luabridge::LuaRef callback, const std::vector<uint32_t>& targets) {
                    if (!callback.isFunction()) {
                        return;
                    }

                    auto mutex = std::make_shared<std::mutex>();
                    lua_State* thread = callback.state();

                    query.run([thread, callback, mutex](size_t count, const Entity* entities_array, const std::vector<void*>& component_blocks) {
                        std::lock_guard<std::mutex> lock(*mutex);

                        luabridge::LuaRef entities = luabridge::newTable(thread);
                        for (size_t index = 0; index < count; ++index) {
                            entities[index + 1] = entities_array[index];
                        }

                        luabridge::LuaRef components = luabridge::newTable(thread);
                        for (size_t index = 0; index < component_blocks.size(); ++index) {
                            components[index + 1] = luabridge::LuaRef(thread, component_blocks[index]);
                        }

                        try {
                            callback(count, entities, components);
                        } catch (const luabridge::LuaException&) {
                        }
                    }, targets);
                })
            .endClass()

            .beginClass<World>("world")
                .addStaticFunction("new", []() { return World(); })
                .addFunction("enroll", &World::enroll)
                .addFunction("spawn", &World::spawn)
                .addFunction("kill", &World::kill)
                .addFunction("has", &World::has)
                .addFunction("detach", &World::detach)
                .addFunction("query", &World::query)
                .addFunction("attach", [](World& world, Entity entity, uint32_t target, const luabridge::LuaRef& data) -> luabridge::LuaRef {
                    lua_State* thread = data.state();
                    const void* pointer = nullptr;

                    if (!data.isNil()) {
                        data.push();
                        pointer = lua_touserdata(thread, -1);
                        lua_pop(thread, 1);
                    }

                    return luabridge::LuaRef(thread, world.attach(entity, target, pointer));
                })
                .addFunction("get", [](World& world, Entity entity, uint32_t target, lua_State* thread) -> luabridge::LuaRef {
                    if (void* memory = world.get(entity, target)) {
                        return luabridge::LuaRef(thread, memory);
                    }
                    return luabridge::LuaRef(thread);
                })
            .endClass()

        .endNamespace();
}