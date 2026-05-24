cmake_minimum_required(VERSION 3.20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(volyx C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(ExternalProject)

find_package(Vulkan REQUIRED)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/uv")
    execute_process(COMMAND git clone --branch v1.48.0 https://github.com/libuv/libuv.git "${CMAKE_CURRENT_SOURCE_DIR}/vendor/uv")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/enet")
    execute_process(COMMAND git clone --depth 1 https://github.com/lsalzman/enet.git "${CMAKE_CURRENT_SOURCE_DIR}/vendor/enet")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/lws")
    execute_process(COMMAND git clone --branch v4.3-stable https://github.com/warmcat/libwebsockets.git "${CMAKE_CURRENT_SOURCE_DIR}/vendor/lws")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bridge")
    execute_process(COMMAND git clone https://github.com/kunitoki/LuaBridge3.git "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bridge")
endif()

file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit/include/luajit-2.1")

if(WIN32)
    ExternalProject_Add(luajit
            GIT_REPOSITORY https://github.com/LuaJIT/LuaJIT.git
            GIT_TAG v2.1
            SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit"
            BUILD_IN_SOURCE TRUE
            CONFIGURE_COMMAND ""
            BUILD_COMMAND cmd /c src/msvcbuild.bat static
            INSTALL_COMMAND ${CMAKE_COMMAND} -E copy src/lua51.lib "${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit/lib/lua51.lib"
            BUILD_BYPRODUCTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit/lib/lua51.lib"
    )
elseif(APPLE)
    ExternalProject_Add(luajit
            GIT_REPOSITORY https://github.com/LuaJIT/LuaJIT.git
            GIT_TAG v2.1
            SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit"
            BUILD_IN_SOURCE TRUE
            CONFIGURE_COMMAND ""
            BUILD_COMMAND env MACOSX_DEPLOYMENT_TARGET=11.0 make -j4 PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit BUILDMODE=static CFLAGS=-fPIC
            INSTALL_COMMAND env MACOSX_DEPLOYMENT_TARGET=11.0 make install PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit
            BUILD_BYPRODUCTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit/lib/libluajit-5.1.a"
    )
else()
    ExternalProject_Add(luajit
            GIT_REPOSITORY https://github.com/LuaJIT/LuaJIT.git
            GIT_TAG v2.1
            SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit"
            BUILD_IN_SOURCE TRUE
            CONFIGURE_COMMAND ""
            BUILD_COMMAND make -j4 PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit BUILDMODE=static CFLAGS=-fPIC
            INSTALL_COMMAND make install PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit
            BUILD_BYPRODUCTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit/lib/libluajit-5.1.a"
    )
endif()

ExternalProject_Add(uv
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/uv"
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/vendor/uv -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBUILD_TESTING=OFF -DLIBUV_BUILD_SHARED=OFF
        BUILD_BYPRODUCTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/uv/lib/libuv.a"
)

ExternalProject_Add(enet
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/enet"
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/vendor/enet -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        BUILD_BYPRODUCTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/enet/lib/libenet.a"
)

ExternalProject_Add(lws
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/lws"
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/vendor/lws
        -DLWS_WITH_SHARED=OFF
        -DLWS_WITH_SSL=OFF
        -DLWS_WITHOUT_TESTAPPS=ON
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        BUILD_BYPRODUCTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/lws/lib/libwebsockets.a"
)

add_library(liblua STATIC IMPORTED GLOBAL)
if(WIN32)
    set_target_properties(liblua PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit/lib/lua51.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit/src"
    )
else()
    set_target_properties(liblua PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit/lib/libluajit-5.1.a"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit/include/luajit-2.1"
    )
endif()
add_dependencies(liblua luajit)

add_library(libuv STATIC IMPORTED GLOBAL)
set_target_properties(libuv PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/vendor/uv/lib/libuv.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/vendor/uv/include"
)
add_dependencies(libuv uv)

add_library(libenet STATIC IMPORTED GLOBAL)
set_target_properties(libenet PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/vendor/enet/lib/libenet.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/vendor/enet/include"
)
add_dependencies(libenet enet)

add_library(liblws STATIC IMPORTED GLOBAL)
set_target_properties(liblws PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/vendor/lws/lib/libwebsockets.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/vendor/lws/include"
)
add_dependencies(liblws lws)

add_library(bridge INTERFACE)
target_include_directories(bridge INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bridge/Source")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_executable(volyx ${SOURCES} ${HEADERS})

set_target_properties(volyx PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

target_include_directories(volyx PUBLIC
        include
        src
        src/vm/api
        src/vm/core
        ${Vulkan_INCLUDE_DIRS}
)

if(WIN32)
    target_link_libraries(volyx PUBLIC bridge liblua libuv libenet liblws Vulkan::Vulkan ws2_32 iphlpapi userenv winmm)
else()
    target_link_libraries(volyx PUBLIC bridge liblua libuv libenet liblws Vulkan::Vulkan m dl pthread)
endif()