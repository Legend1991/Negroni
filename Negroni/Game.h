#pragma once

#include "PlayerScript.h"
#include "EditorScript.h"
//#include "PhysX.h"

#include "Core/Base.h"
#include "Core/GameLoop.h"
#include "Core/Logger.h"
#include "Core/Keyboard.h"
//#include "Core/CubeMesh.h"
#include "Core/Asset.h"
#include "Assimp/AssimpMeshLoader.h"

#include <omp.h>
#include <chrono>

using namespace Core;
//using namespace LMD;

namespace Negroni
{
    class Game : public GameLoop
    {
    public:
        Game()
        {
            Asset::UseMeshLoader(MakeRef<Assimp::AssimpMeshLoader>(Assimp::MeshReadFlag::DirectX));
            Asset::LoadMesh("Cube", "Assets\\Models\\Cube.glb");
            Asset::LoadMesh("Capsule", "Assets\\Models\\Capsule.glb");
            Asset::LoadMesh("Plane", "Assets\\Models\\Plane.glb");
            Asset::LoadMesh("Sphere", "Assets\\Models\\Sphere.glb");

            //------------------------------------------------------------------

            auto& cube = state.objects.emplace_back(MakeRef<Object>());
            cube->script = MakeRef<PlayerScript>(cube);
            cube->selected = true;

            //auto& plane = state.objects.emplace_back(MakeRef<Object>());
            //plane->mesh = Asset::GetMesh("Plane");
            //plane->transform.location = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
            //plane->tintColor[0] = 97.0f / 255;
            //plane->tintColor[1] = 126.0f / 255;
            //plane->tintColor[2] = 58.0f / 255;

            auto& smallCube = state.objects.emplace_back(MakeRef<Object>());
            smallCube->transform.location = { .x = -1.0f, .y = 0.0f, .z = 0.0f };
            smallCube->transform.scale = { .x = 1.0f, .y = 1.0f, .z = 1.0f };
            smallCube->mesh = Asset::GetMesh("Cube");
            smallCube->tintColor[0] = 172.0f / 255;
            smallCube->tintColor[1] = 209.0f / 255;
            smallCube->tintColor[2] = 126.0f / 255;

            auto& smallCube1 = state.objects.emplace_back(MakeRef<Object>());
            smallCube1->transform.location = { .x = 1.0f, .y = 0.0f, .z = 0.0f };
            smallCube1->transform.scale = { .x = 1.0f, .y = 1.0f, .z = 1.0f };
            smallCube1->mesh = Asset::GetMesh("Cube");
            smallCube1->tintColor[0] = 152.0f / 255;
            smallCube1->tintColor[1] = 46.0f / 255;
            smallCube1->tintColor[2] = 59.0f / 255;
            //smallCube1->tintColor[0] = 172.0f / 255;
            //smallCube1->tintColor[1] = 209.0f / 255;
            //smallCube1->tintColor[2] = 126.0f / 255;

            for (int x = 0; x < 11; ++x)
            {
                for (int y = 0; y < 10; ++y)
                {
                    auto& o = state.objects.emplace_back(MakeRef<Object>());
                    o->transform.location = { .x = (x - 5.0f), .y = -1.0f, .z = (y - 1.0f) };
                    o->transform.scale = { .x = 1.0f, .y = 1.0f, .z = 1.0f };
                    o->mesh = Asset::GetMesh("Cube");
                    o->tintColor[0] = 0.0f;
                    o->tintColor[1] = 130.0f / 255.0f;
                    o->tintColor[2] = 54.0f / 255.0f;
                }
            }

#if defined(DEVELOPER)
            auto& editor = state.objects.emplace_back(MakeRef<Object>());
            editor->script = MakeRef<EditorScript>(state.objects);
#endif

            numThreads = std::thread::hardware_concurrency() / 2;
            //numThreads = omp_get_max_threads() / 2;
            log_info("threads in use: {}", numThreads);

            //objects.reserve(1'00'000);
            //for (int i = 0; i < objects.capacity(); ++i)
            //{
            //    objects.push_back(MakeScope<Cube>());
            //}

            //physics.Add(&cube);
        }

    private:
        //PhysX     physics;
        u8        numThreads;
    };
}
