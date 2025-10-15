#pragma once

#include "Physics.h"
#include "ID.h"
#include "Logger.h"

#include <memory>
#include <string>
#include <format>
#include <vector>

namespace Core
{
    //------------------------------------------------------------------------------------
    // Typedefs
    //------------------------------------------------------------------------------------

    typedef __int8  i8;
    typedef __int16 i16;
    typedef __int32 i32;
    typedef __int64 i64;

    typedef unsigned __int8  u8;
    typedef unsigned __int16 u16;
    typedef unsigned __int32 u32;
    typedef unsigned __int64 u64;

    typedef float  r32;
    typedef double r64;

    typedef std::string  str;
    typedef std::wstring wstr;
    typedef const char   *cstr;

    typedef float *rgba;
    typedef float RGB[3];

    //------------------------------------------------------------------------------------
    // Refs
    //------------------------------------------------------------------------------------

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Scope<T> MakeScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Ref<T> MakeRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Weak = std::weak_ptr<T>;

    //------------------------------------------------------------------------------------
    // Primitives
    //------------------------------------------------------------------------------------

    struct Vec2f { float x, y; };
    struct Vec3f { float x, y, z; };
    struct Rot3f { float pitch, yaw, roll; };

    struct Transform
    {
        Vec3f location = { 0.0f, 0.0f, 0.0f };
        Rot3f rotation = { 0.0f, 0.0f, 0.0f };
        Vec3f scale    = { 1.0f, 1.0f, 1.0f };
    };

    struct Vertex
    {
        Vec3f position;
        Vec3f normal;
        Vec2f texCoord;
    };

    struct Sphere
    {
        Vec3f center;
        float radius = 1.0f;
    };

    struct Mesh
    {
        ID                  id;
        std::string         name;
        std::vector<Vertex> vertices;
        std::vector<u16>    indices;
        Sphere              boundingSphere;
        void*               data = nullptr;
    };

    typedef Ref<Mesh>	MeshRef;

    class RigidBody
    {
    public:
        virtual void AddForce(const Vec3f& value) = 0;
        virtual void AddImpulse(const Vec3f& value) = 0;
        virtual void AddVelocityChange(const Vec3f& value) = 0;
        virtual void AddAcceleration(const Vec3f& value) = 0;
        virtual bool IsSleeping() = 0;
    };

    typedef Ref<RigidBody>	RigidBodyRef;

    class Object;

    class Physics
    {
    public:
        virtual void Update(float dt) = 0;
        virtual void Add(Object* object) = 0;
    };

    class Script
    {
    public:
        virtual void FixedUpdate() = 0;
        virtual void Update(float) = 0;
        virtual str Name() = 0;
    };

    class NullScript : public Script
    {
    public:
        virtual void FixedUpdate() override {}
        virtual void Update(float) override {}
        virtual str Name() override { return "<NULL>"; }
    };

    typedef Ref<Script>	ScriptRef;

    class Object
    {
    public:
        ID             id;
        Transform      transform;
        bool           useTintColor = false;
        RGB            tintColor = { 1.0f, 1.0f, 1.0f };
        RigidBodyRef   body;
        ScriptRef      script = MakeRef<NullScript>();
        MeshRef        mesh;
        bool           selected = false;
    };

    typedef Ref<Object>	ObjectRef;

    //------------------------------------------------------------------------------------
    // String
    //------------------------------------------------------------------------------------

    static void LTrim(str& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](u8 ch) {
            return !std::isspace(ch);
        }));
    }

    static void RTrim(str& s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](u8 ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }
}

//----------------------------------------------------------------------------------------
// Formatters
//----------------------------------------------------------------------------------------

template <>
struct std::formatter<Core::Vec3f> : std::formatter<std::string>
{
    auto format(Core::Vec3f vec, format_context& context) const {
        return formatter<std::string>::format(
            std::format("({:.2f}, {:.2f}, {:.2f})", vec.x, vec.y, vec.z), context
        );
    }
};

template <>
struct std::formatter<Core::Rot3f> : std::formatter<std::string>
{
    auto format(Core::Rot3f rot, format_context& context) const {
        return formatter<std::string>::format(
            std::format("({:.2f}, {:.2f}, {:.2f})", rot.pitch, rot.yaw, rot.roll), context
        );
    }
};

template <>
struct std::formatter<Core::Transform> : std::formatter<std::string>
{
    auto format(Core::Transform tran, format_context& context) const {
        return formatter<std::string>::format(
            std::format("loc: {}    rot: {}", tran.location, tran.rotation), context
        );
    }
};
