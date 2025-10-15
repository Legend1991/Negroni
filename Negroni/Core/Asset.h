#pragma once

#include "MeshLoader.h"

#include <map>

namespace Core
{
	class Asset
	{
	public:
		static void UseMeshLoader(Ref<MeshLoader> meshLoader);
		static void LoadMesh(str meshName, str fileName);
		static MeshRef GetMesh(str meshName);

	private:
		static Ref<MeshLoader> meshLoader;
		static std::map<str, MeshRef> meshes;
	};
}
