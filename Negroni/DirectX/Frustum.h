#pragma once

#include "../Core/Base.h"
#include <directxmath.h>

using namespace Core;

namespace DirectX
{
	class Frustum
	{
	public:
		Frustum(float, XMMATRIX, XMMATRIX);

		bool CheckPoint(float, float, float);
		bool CheckCube(float, float, float, float);
		bool CheckSphere(float, float, float, float);
		bool CheckRectangle(float, float, float, float, float, float);

		bool CheckSphere(const ObjectRef&);

	private:
		XMVECTOR planes[6];
	};
}
