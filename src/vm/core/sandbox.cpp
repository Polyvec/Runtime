#include "../include/vm/core/sandbox.hpp"
#include <cstdio>
#include <cassert>
#include <new>
#include <stdexcept>

extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>

void register_ecs_bindings(lua_State* state);
void register_math_bindings(lua_State* state);

#if defined(_WIN32)
#include <windows.h>
static void confine() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;
    HANDLE jobHandle = CreateJobObject(nullptr, nullptr);
    if (!jobHandle) return;
    JOBOBJECT_BASIC_LIMIT_INFORMATION limitInformation = {};
    limitInformation.LimitFlags = JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
    limitInformation.ActiveProcessLimit = 1;
    SetInformationJobObject(jobHandle, JobObjectBasicLimitInformation, &limitInformation, sizeof(limitInformation));
    AssignProcessToJobObject(jobHandle, GetCurrentProcess());
}
#else
#include <sys/resource.h>
static void confine() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;
    rlimit resourceLimit{};
    resourceLimit.rlim_cur = 8;
    resourceLimit.rlim_max = 8;
    setrlimit(RLIMIT_NOFILE, &resourceLimit);
    resourceLimit.rlim_cur = 0;
    resourceLimit.rlim_max = 0;
    setrlimit(RLIMIT_NPROC, &resourceLimit);
}
#endif

static void govern(lua_State* state, const lua_Debug* debugInfo) {
    (void)debugInfo;
    lua_sethook(state, reinterpret_cast<lua_Hook>(govern), LUA_MASKCOUNT, 1);
    luaL_error(state, "sandbox: cpu budget exceeded");
}

namespace sandbox {

Sandbox::Sandbox() {
    try {
        confine();

        memory = new(std::nothrow) allocator::Allocator(MEMORY);
        assert(memory != nullptr);

        state = lua_newstate(allocator::Allocator::manage, memory);
        if (!state) {
            throw std::runtime_error("Failed to initialize Lua state.");
        }

        luaL_openlibs(state);

        register_ecs_bindings(state);
        register_math_bindings(state);
    }
    catch (const std::exception& exception) {
        std::fprintf(stderr, "sandbox initialization error: %s\n", exception.what());

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

int Sandbox::run(const char* filePath) const {
    try {
        lua_State* contextState = state;
        if (!contextState) {
            std::fprintf(stderr, "sandbox error: Lua state is uninitialized\n");
            return 0;
        }

        lua_State* threadState = lua_newthread(contextState);
        if (!threadState) {
            std::fprintf(stderr, "sandbox error: Failed to create Lua coroutine thread\n");
            return 0;
        }

        lua_sethook(threadState, reinterpret_cast<lua_Hook>(govern), LUA_MASKCOUNT, INSTRUCTIONS);

        if (luaL_loadfile(threadState, filePath) != LUA_OK) {
            std::fprintf(stderr, "sandbox error: %s\n", lua_tostring(threadState, -1));
            lua_pop(contextState, 1);
            return 0;
        }

        if (lua_pcall(threadState, 0, 0, 0) != LUA_OK) {
            std::fprintf(stderr, "runtime error: %s\n", lua_tostring(threadState, -1));
            lua_pop(contextState, 1);
            return 0;
        }

        lua_pop(contextState, 1);
        return 1;
    }
    catch (const std::exception& exception) {
        std::fprintf(stderr, "unexpected C++ exception in sandbox run: %s\n", exception.what());
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