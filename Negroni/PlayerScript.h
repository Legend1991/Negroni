#pragma once

#include "Core/Base.h"
#include "Core/Logger.h"
#include "Core/Input.h"
#include "Core/Keyboard.h"
#include "Core/Mouse.h"
#include "Core/CubeMesh.h"
#include "Core/Asset.h"

#include <cassert>
#include <omp.h>

using namespace Core;

namespace Negroni
{
    class PlayerScript : public Script
    {
        struct Gravity
        {
            float velocity;
            float weight;
            float rotationSpeed;
            float moveSpeed;
        };

        float objectOrigin = 0.0f;

    public:
        PlayerScript(const ObjectRef& owner) : object(owner)
        {
            rotationDirection = 1;
            object->transform.location = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
            object->transform.scale = { .x = 1.0f, .y = 1.0f, .z = 1.0f };
            object->useTintColor = true;
            object->tintColor[0] = 0.2f;
            object->tintColor[1] = 0.4f;
            object->tintColor[2] = 1.0f;

            object->mesh = Asset::GetMesh("Capsule");

            memset(&gravity, 0, sizeof(Gravity));
            gravity.velocity = 0.0f;
            gravity.weight = 30.0f;
            gravity.rotationSpeed = 300.0f;

            Keyboard::OnPress("Jump") = std::bind(&PlayerScript::Jump, this);
            Keyboard::OnPress("ReverseRotation") = std::bind(&PlayerScript::ReverseRotation, this);

            log_info("player script consturctor");
        }

        void Jump()
        {
            //assert(body != nullptr);

            //if (body->IsSleeping())
            //{
            //    log_info("jump");
            //    body->AddVelocityChange({ .x = 0.0f, .y = 10.0f, .z = 0.0f });
            //}

            if (IsJumping())
            {
                return;
            }

            gravity.velocity = 10.0f;
        }

        void ReverseRotation()
        {
            rotationDirection *= -1;
        }

        void FixedUpdate() override
        {
        }

        void Update(float dt) override
        {
            if (IsJumping())
            {
                gravity.velocity -= gravity.weight * dt;
                object->transform.location.y += gravity.velocity * dt;

                if (object->transform.location.y + objectOrigin < 0.0f)
                {
                    object->transform.location.y = 0.0f - objectOrigin;
                    gravity.velocity = 0.0f;
                }
            }

            float moveSpeed = 3.0f * dt;

            if (Input::IsDown("MoveForward"))
            {
                object->transform.location.z += moveSpeed;
            }
            if (Input::IsDown("MoveBackward"))
            {
                object->transform.location.z -= moveSpeed;
            }
            if (Input::IsDown("MoveLeft"))
            {
                object->transform.location.x -= moveSpeed;
            }
            if (Input::IsDown("MoveRight"))
            {
                object->transform.location.x += moveSpeed;
            }

            const float pitch = object->transform.rotation.pitch + gravity.rotationSpeed * dt * rotationDirection;
            object->transform.rotation.pitch = pitch >= 360.0f ? 0.0f : (pitch <= -360.0f ? 0.0f : pitch);
        }

        virtual str Name() override { return "PlayerScript"; }

        bool IsJumping() const
        {
            return object->transform.location.y + objectOrigin > 0.0f || gravity.velocity > 0.0f;
        }

        ObjectRef object;
        Gravity   gravity;
        i8        rotationDirection;
    };
}
