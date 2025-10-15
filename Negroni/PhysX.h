#pragma once

#include "Core/Base.h"

#include "PxPhysicsAPI.h"

using namespace Core;
using namespace physx;

namespace LMD
{
    class PhysXRigidBody : public RigidBody
    {
    public:
        PhysXRigidBody(PxRigidDynamic* body);

        virtual void AddForce(const Vec3f& value) override;
        virtual void AddImpulse(const Vec3f& value) override;
        virtual void AddVelocityChange(const Vec3f& value) override;
        virtual void AddAcceleration(const Vec3f& value) override;
        virtual bool IsSleeping() override;

    private:
        PxRigidDynamic* body;
    };

    class PhysX : public Physics
    {
    public:
        PhysX();
        ~PhysX();

        virtual void Add(Object* object) override;
        virtual void Update(float dt) override;

    private:
        PxRigidDynamic* createCube(Object* object);
        float roll(const PxQuat& q);
        float pitch(const PxQuat& q);
        float yaw(const PxQuat& q);
        float rad2deg(float rad);
        Rot3f quat2rot(const PxQuat& q);

        PxDefaultAllocator		allocator;
        PxDefaultErrorCallback	errorCallback;
        PxFoundation*           foundation = nullptr;
        PxPhysics*              physics = nullptr;
        PxDefaultCpuDispatcher* dispatcher = nullptr;
        PxScene*                scene = nullptr;
        PxMaterial*             material = nullptr;
    };
}
