#pragma once

#include "Base.h"

#include <float.h>

namespace Core
{
	class MeshLoader
	{
	public:
		virtual MeshRef LoadMesh(str fileName) = 0;

	protected:
		void registerVertexPosition(const Vec3f& vertex);
		void resetBoundingBox();
		Vec3f calculateBoundingCenter() const;
		float calculateBoundingRadius() const;

	private:
		float minX = FLT_MAX;
		float maxX = FLT_MIN;
		float minY = FLT_MAX;
		float maxY = FLT_MIN;
		float minZ = FLT_MAX;
		float maxZ = FLT_MIN;
	};
}
