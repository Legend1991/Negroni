#include "Asset.h"

#include <cassert>

namespace Core
{

    Ref<MeshLoader> Asset::meshLoader = nullptr;
    std::map<str, MeshRef> Asset::meshes;

    void Asset::UseMeshLoader(Ref<MeshLoader> meshLoader)
    {
        Asset::meshLoader = meshLoader;
    }

    void Asset::LoadMesh(str meshName, str fileName)
    {
        assert(meshLoader);
        assert(!meshes.contains(meshName));

        MeshRef mesh = meshLoader->LoadMesh(fileName);
        mesh->name = meshName;
        meshes[meshName] = mesh;
    }

    MeshRef Asset::GetMesh(str meshName)
    {
        assert(meshLoader);
        assert(meshes.contains(meshName));

        return meshes[meshName];
    }
}
