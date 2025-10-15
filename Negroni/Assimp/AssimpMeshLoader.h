#pragma once

#include "../Core/Base.h"
#include "../Core/MeshLoader.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

using namespace Core;

namespace Assimp
{
    enum MeshReadFlag
    {
        DirectX = aiProcess_Triangulate | aiProcess_ConvertToLeftHanded,
        OpenGL = aiProcessPreset_TargetRealtime_Quality,
    };

    class AssimpMeshLoader : public MeshLoader
    {
    public:
        AssimpMeshLoader(MeshReadFlag readFlag);
        virtual MeshRef LoadMesh(str fileName) override;

    private:
        MeshReadFlag readFlag;
    };
}
