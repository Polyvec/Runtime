#include "../include/vm/core/sandbox.hpp"
#include <cstdio>
#include <cassert>
#include <new>
#include <stdexcept>
#include <string>

extern "C" {
#include <lauxlib.h>
}
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

void ecs(lua_State* state);
void types(lua_State* state);

#if defined(_WIN32)
#include <windows.h>
static void confine() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;
    HANDLE job = CreateJobObject(nullptr, nullptr);
    if (!job) return;
    JOBOBJECT_BASIC_LIMIT_INFORMATION limit = {};
    limit.LimitFlags = JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
    limit.ActiveProcessLimit = 1;
    SetInformationJobObject(job, JobObjectBasicLimitInformation, &limit, sizeof(limit));
    AssignProcessToJobObject(job, GetCurrentProcess());
}
#else
#include <sys/resource.h>
static void confine() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;
    rlimit limit{};
    limit.rlim_cur = 8;
    limit.rlim_max = 8;
    setrlimit(RLIMIT_NOFILE, &limit);
    limit.rlim_cur = 0;
    limit.rlim_max = 0;
    setrlimit(RLIMIT_NPROC, &limit);
}
#endif

static void govern(lua_State* state, const lua_Debug* debug) {
    (void)debug;
    lua_sethook(state, reinterpret_cast<lua_Hook>(govern), LUA_MASKCOUNT, 1);
    luaL_error(state, "sandbox: cpu budget exceeded");
}

namespace sandbox {

Sandbox::Sandbox() {
    try {
        memory = new(std::nothrow) allocator::Allocator(MEMORY);
        assert(memory != nullptr);

        state = lua_newstate(allocator::Allocator::manage, memory);
        if (!state) {
            throw std::runtime_error("Failed to initialize Lua state.");
        }

        sol::state_view view(state);
        view.set_panic(sol::default_at_panic);

        view.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::string, sol::lib::math);

        ecs(state);
        types(state);
    }
    catch (const std::exception& error) {
        std::fprintf(stderr, "sandbox initialization error: %s\n", error.what());

        if (state) {
            lua_close(state);
            state = nullptr;
        }
        delete memory;
        memory = nullptr;
    }
    catch (...) {
        std::fprintf(stderr, "sandbox initialization error: Unknown exception caught.\n");
        if (state) {
            lua_close(state);
            state = nullptr;
        }
        delete memory;
        memory = nullptr;
    }
}

Sandbox::~Sandbox() {
    if (state) lua_close(state);
    delete memory;
}

int Sandbox::run(const char* path) const {
    try {
        lua_State* master = state;
        if (!master) {
            std::fprintf(stderr, "sandbox error: Lua state is uninitialized\n");
            return 0;
        }

        confine();

        lua_State* thread = lua_newthread(master);
        if (!thread) {
            std::fprintf(stderr, "sandbox error: Failed to create Lua coroutine thread\n");
            return 0;
        }

        lua_sethook(thread, reinterpret_cast<lua_Hook>(govern), LUA_MASKCOUNT, INSTRUCTIONS);

        sol::state_view view(thread);

        sol::load_result load = view.load_file(path);
        if (!load.valid()) {
            sol::error error = load;
            std::fprintf(stderr, "sandbox error: %s\n", error.what());
            lua_pop(master, 1);
            return 0;
        }

        sol::protected_function function = load;
        sol::protected_function_result call = function();

        if (!call.valid()) {
            sol::error error = call;
            std::fprintf(stderr, "runtime error: %s\n", error.what());
            lua_pop(master, 1);
            return 0;
        }

        lua_pop(master, 1);
        return 1;
    }
    catch (const std::exception& error) {
        std::fprintf(stderr, "unexpected C++ exception in sandbox run: %s\n", error.what());
        return 0;
    }
    catch (...) {
        std::fprintf(stderr, "unexpected unknown exception in sandbox run\n");
        if (state) {
            lua_settop(state, 0);
        }
        return 0;
    }
}

}