#include "../../../include/vm/api/math.hpp"
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <string>

#include "../../../include/math/vector3.hpp"
#include "../../../include/math/vector4.hpp"
#include "../../../include/math/quaternion.hpp"
#include "../../../include/math/matrix4.hpp"
#include "../../../include/math/transform.hpp"

using namespace voxyl::math;

template <typename Type>
void metamethod(lua_State* state, lua_CFunction function) {
    (void)luabridge::Stack<Type>::push(state, Type());
    if (lua_getmetatable(state, -1)) {
        lua_pushcfunction(state, function);
        lua_setfield(state, -2, "__mul");
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}

void register_math_bindings(lua_State* state) {
    luabridge::getGlobalNamespace(state)
        .beginNamespace("math")

            .beginClass<Vector3>("vector3")
                .addConstructor<void()>()
                .addConstructor<void(float, float, float)>()
                .addProperty("x", &Vector3::x)
                .addProperty("y", &Vector3::y)
                .addProperty("z", &Vector3::z)
                .addFunction("length", &Vector3::length)
                .addFunction("normalized", &Vector3::normalized)
                .addFunction("cross", &Vector3::cross)
                .addFunction("__add", [](const Vector3& alpha, const Vector3& beta) {
                    return alpha + beta;
                })
                .addFunction("__sub", [](const Vector3& alpha, const Vector3& beta) {
                    return alpha - beta;
                })
                .addFunction("__tostring", [](const Vector3& alpha) {
                    char output[128];
                    std::snprintf(output, sizeof(output), "Vector3(%f, %f, %f)", static_cast<double>(alpha.x), static_cast<double>(alpha.y), static_cast<double>(alpha.z));
                    return std::string(output);
                })
            .endClass()

            .beginClass<Quaternion>("quaternion")
                .addConstructor<void()>()
                .addConstructor<void(float, float, float, float)>()
                .addProperty("x", &Quaternion::x)
                .addProperty("y", &Quaternion::y)
                .addProperty("z", &Quaternion::z)
                .addProperty("w", &Quaternion::w)
                .addFunction("euler", static_cast<Vector3 (Quaternion::*)() const>(&Quaternion::euler))
            .endClass()

            .beginClass<Vector4>("vector4")
                .addConstructor<void()>()
                .addConstructor<void(float, float, float, float)>()
                .addProperty("x", &Vector4::x)
                .addProperty("y", &Vector4::y)
                .addProperty("z", &Vector4::z)
                .addProperty("w", &Vector4::w)
                .addFunction("length", &Vector4::length)
                .addFunction("normalized", &Vector4::normalized)
                .addFunction("__add", [](const Vector4& alpha, const Vector4& beta) {
                    return alpha + beta;
                })
                .addFunction("__sub", [](const Vector4& alpha, const Vector4& beta) {
                    return alpha - beta;
                })
                .addFunction("__tostring", [](const Vector4& alpha) {
                    char output[128];
                    std::snprintf(output, sizeof(output), "Vector4(%f, %f, %f, %f)", static_cast<double>(alpha.x), static_cast<double>(alpha.y), static_cast<double>(alpha.z), static_cast<double>(alpha.w));
                    return std::string(output);
                })
            .endClass()

            .beginClass<Matrix4>("matrix4")
                .addConstructor<void()>()
                .addConstructor<void(float)>()
                .addFunction("transposed", &Matrix4::transposed)
                .addFunction("inverted", &Matrix4::inverted)
                .addFunction("__add", [](const Matrix4& alpha, const Matrix4& beta) {
                    return alpha + beta;
                })
                .addFunction("__sub", [](const Matrix4& alpha, const Matrix4& beta) {
                    return alpha - beta;
                })
                .addFunction("__tostring", [](const Matrix4& alpha) {
                    char output[256];
                    std::snprintf(output, sizeof(output), "Matrix4([[ %f, %f, %f, %f ], ...])",
                                 static_cast<double>(alpha(0,0)), static_cast<double>(alpha(0,1)), static_cast<double>(alpha(0,2)), static_cast<double>(alpha(0,3)));
                    return std::string(output);
                })
            .endClass()

            .beginClass<Transform>("transform")
                .addConstructor<void()>()
                .addConstructor<void(const Vector3&, const Quaternion&, const Vector3&)>()
                .addProperty("translation", &Transform::translation)
                .addProperty("rotation", &Transform::rotation)
                .addProperty("scale", &Transform::scale)
                .addFunction("matrix", &Transform::matrix)
                .addFunction("__mul", [](const Transform& alpha, const Transform& beta) {
                    return alpha * beta;
                })
                .addFunction("__tostring", [](const Transform& alpha) {
                    (void)alpha;
                    return std::string("Transform()");
                })
            .endClass()

        .endNamespace();

    metamethod<Vector3>(state, [](lua_State* L) -> int {
        if (luabridge::Stack<Vector3>::isInstance(L, 1) && luabridge::Stack<Vector3>::isInstance(L, 2)) {
            (void)luabridge::Stack<Vector3>::push(L, luabridge::Stack<Vector3>::get(L, 1).value() * luabridge::Stack<Vector3>::get(L, 2).value());
            return 1;
        }
        if (luabridge::Stack<Vector3>::isInstance(L, 1) && lua_isnumber(L, 2)) {
            (void)luabridge::Stack<Vector3>::push(L, luabridge::Stack<Vector3>::get(L, 1).value() * static_cast<float>(lua_tonumber(L, 2)));
            return 1;
        }
        if (lua_isnumber(L, 1) && luabridge::Stack<Vector3>::isInstance(L, 2)) {
            (void)luabridge::Stack<Vector3>::push(L, luabridge::Stack<Vector3>::get(L, 2).value() * static_cast<float>(lua_tonumber(L, 1)));
            return 1;
        }
        return luaL_error(L, "Invalid operands for Vector3 multiplication");
    });

    metamethod<Quaternion>(state, [](lua_State* L) -> int {
        if (luabridge::Stack<Quaternion>::isInstance(L, 1) && luabridge::Stack<Quaternion>::isInstance(L, 2)) {
            (void)luabridge::Stack<Quaternion>::push(L, luabridge::Stack<Quaternion>::get(L, 1).value() * luabridge::Stack<Quaternion>::get(L, 2).value());
            return 1;
        }
        if (luabridge::Stack<Quaternion>::isInstance(L, 1) && luabridge::Stack<Vector3>::isInstance(L, 2)) {
            (void)luabridge::Stack<Vector3>::push(L, luabridge::Stack<Quaternion>::get(L, 1).value() * luabridge::Stack<Vector3>::get(L, 2).value());
            return 1;
        }
        if (luabridge::Stack<Quaternion>::isInstance(L, 1) && lua_isnumber(L, 2)) {
            (void)luabridge::Stack<Quaternion>::push(L, luabridge::Stack<Quaternion>::get(L, 1).value() * static_cast<float>(lua_tonumber(L, 2)));
            return 1;
        }
        if (lua_isnumber(L, 1) && luabridge::Stack<Quaternion>::isInstance(L, 2)) {
            (void)luabridge::Stack<Quaternion>::push(L, luabridge::Stack<Quaternion>::get(L, 2).value() * static_cast<float>(lua_tonumber(L, 1)));
            return 1;
        }
        return luaL_error(L, "Invalid operands for Quaternion multiplication");
    });

    metamethod<Vector4>(state, [](lua_State* L) -> int {
        if (luabridge::Stack<Vector4>::isInstance(L, 1) && lua_isnumber(L, 2)) {
            (void)luabridge::Stack<Vector4>::push(L, luabridge::Stack<Vector4>::get(L, 1).value() * static_cast<float>(lua_tonumber(L, 2)));
            return 1;
        }
        if (lua_isnumber(L, 1) && luabridge::Stack<Vector4>::isInstance(L, 2)) {
            (void)luabridge::Stack<Vector4>::push(L, luabridge::Stack<Vector4>::get(L, 2).value() * static_cast<float>(lua_tonumber(L, 1)));
            return 1;
        }
        return luaL_error(L, "Invalid operands for Vector4 multiplication");
    });

    metamethod<Matrix4>(state, [](lua_State* L) -> int {
        if (luabridge::Stack<Matrix4>::isInstance(L, 1) && luabridge::Stack<Matrix4>::isInstance(L, 2)) {
            (void)luabridge::Stack<Matrix4>::push(L, luabridge::Stack<Matrix4>::get(L, 1).value() * luabridge::Stack<Matrix4>::get(L, 2).value());
            return 1;
        }
        if (luabridge::Stack<Matrix4>::isInstance(L, 1) && luabridge::Stack<Vector4>::isInstance(L, 2)) {
            (void)luabridge::Stack<Vector4>::push(L, luabridge::Stack<Matrix4>::get(L, 1).value() * luabridge::Stack<Vector4>::get(L, 2).value());
            return 1;
        }
        return luaL_error(L, "Invalid operands for Matrix4 multiplication");
    });
}