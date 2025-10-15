#include "PhysX.h"

#include "Core/Logger.h"

#include <algorithm>
#include <numbers>

//----------------------------------------------------------------------------------------------------------------------
// PhysX RigidBody adapter
//----------------------------------------------------------------------------------------------------------------------

LMD::PhysXRigidBody::PhysXRigidBody(PxRigidDynamic* body) : body(body)
{
}

void LMD::PhysXRigidBody::AddForce(const Vec3f& value)
{
    body->addForce(*(PxVec3*)&value, PxForceMode::eFORCE);
}

void LMD::PhysXRigidBody::AddImpulse(const Vec3f& value)
{
    body->addForce(*(PxVec3*)&value, PxForceMode::eIMPULSE);
}

void LMD::PhysXRigidBody::AddVelocityChange(const Vec3f& value)
{
    body->addForce(*(PxVec3*)&value, PxForceMode::eVELOCITY_CHANGE);
}

void LMD::PhysXRigidBody::AddAcceleration(const Vec3f& value)
{
    body->addForce(*(PxVec3*)&value, PxForceMode::eACCELERATION);
}

bool LMD::PhysXRigidBody::IsSleeping()
{
    return body->isSleeping();
}

//----------------------------------------------------------------------------------------------------------------------
// PhysX system adapter
//----------------------------------------------------------------------------------------------------------------------

LMD::PhysX::PhysX()
{
    foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback);

    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true);

    PxSceneDesc sceneDesc(physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    dispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = dispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    scene = physics->createScene(sceneDesc);

    material = physics->createMaterial(0.5f, 0.5f, 0.25f);

    PxRigidStatic* groundPlane = PxCreatePlane(*physics, PxPlane(0, 1, 0, 0), *material);
    scene->addActor(*groundPlane);
}

LMD::PhysX::~PhysX()
{
    PX_RELEASE(scene);
    PX_RELEASE(dispatcher);
    PX_RELEASE(physics);
    PX_RELEASE(foundation);
}

void LMD::PhysX::Add(Object* object)
{
    PxRigidDynamic* cube = createCube(object);
}

void LMD::PhysX::Update(float dt)
{
    scene->simulate(dt);
    scene->fetchResults(true);

    // Update each game object
    PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
    if (!nbActors) return;

    PxArray<PxRigidActor*> actors(nbActors);
    PxActorTypeFlags types = PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC;
    scene->getActors(types, reinterpret_cast<PxActor**>(&actors[0]), nbActors);

    PxShape* shapes[128];

    for (PxU32 i = 0; i < actors.size(); i++)
    {
        const PxRigidActor* actor = actors[i];

        bool sleeping = actor->is<PxRigidDynamic>() ? actor->is<PxRigidDynamic>()->isSleeping() : false;
        if (sleeping) continue;

        const PxU32 nbShapes = actor->getNbShapes();
        actor->getShapes(shapes, nbShapes);

        for (PxU32 j = 0; j < nbShapes; j++)
        {
            const PxGeometry& geom = shapes[j]->getGeometry();
            if (geom.getType() == PxGeometryType::ePLANE) continue;

            Object* object = actor->userData ? (Object*)actor->userData : nullptr;
            if (!object) continue;

            const PxTransform shapePose = PxShapeExt::getGlobalPose(*shapes[j], *actor);

            object->transform.location = *(Vec3f*)&shapePose.p; // TODO: set origin to the bottom of the object
            object->transform.rotation = quat2rot(shapePose.q);

            log_info("transform: location{}", object->transform.location);
        }
    }
}

PxRigidDynamic* LMD::PhysX::createCube(Object* object)
{
    const Vec3f& loc = object->transform.location;
    const Vec3f& scale = object->transform.scale;
    const PxTransform t(PxVec3(loc.x, loc.y, loc.z));

    PxTransform localTm(PxVec3(0, 0, 0) * scale.x);
    PxRigidDynamic* body = physics->createRigidDynamic(t.transform(localTm));
    PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);

    PxShape* shape = physics->createShape(PxBoxGeometry(scale.x, scale.y, scale.z), *material);
    body->attachShape(*shape);
    shape->release();

    scene->addActor(*body);

    body->userData = (void*)object;
    object->body = new PhysXRigidBody(body);

    return body;
}

//----------------------------------------------------------------------------------------------------------------------
// Utilities
//----------------------------------------------------------------------------------------------------------------------

float LMD::PhysX::roll(const PxQuat& q)
{
    float y = 2.0f * (q.x * q.y + q.w * q.z);
    float x = q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z;

    constexpr float epsilon = std::numeric_limits<float>::epsilon();
    if (std::abs(y) <= epsilon && std::abs(x) <= epsilon)
    {
        return 0.0f;
    }

    return std::atan2(y, x);
}

float LMD::PhysX::pitch(const PxQuat& q)
{
    float y = 2.0f * (q.y * q.z + q.w * q.x);
    float x = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;

    constexpr float epsilon = std::numeric_limits<float>::epsilon();
    if (std::abs(y) <= epsilon && std::abs(x) <= epsilon)
    {
        return 2.0f * std::atan2(q.x, q.w);
    }

    return std::atan2(y, x);
}

float LMD::PhysX::yaw(const PxQuat& q)
{
    return std::asin(std::clamp(-2.0f * (q.x * q.z - q.w * q.y), -1.0f, 1.0f));
}

float LMD::PhysX::rad2deg(float rad)
{
    return rad * 180.0f / std::numbers::pi;
}

Rot3f LMD::PhysX::quat2rot(const PxQuat& q)
{
    return {
        .pitch = rad2deg(pitch(q)),
        .yaw = rad2deg(yaw(q)),
        .roll = rad2deg(roll(q))
    };
}
