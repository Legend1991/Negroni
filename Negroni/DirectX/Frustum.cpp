#include "Frustum.h"

#include <algorithm>

using namespace DirectX;

DirectX::Frustum::Frustum(float screenDepth, XMMATRIX projection, XMMATRIX view)
{
	// Calculate the minimum Z distance in the frustum.
	float zMinimum = -XMVectorGetZ(projection.r[3]) / XMVectorGetZ(projection.r[2]);
	float r = screenDepth / (screenDepth - zMinimum);

	// Update projection matrix elements
	XMVECTOR newRow2 = XMVectorSetZ(projection.r[2], r);
	XMVECTOR newRow3 = XMVectorSetZ(projection.r[3], -r * zMinimum);
	projection.r[2] = newRow2;
	projection.r[3] = newRow3;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	XMMATRIX matrix = XMMatrixMultiply(view, projection);

	// Extract columns from the matrix for plane calculations
	XMVECTOR col0 = XMVectorSet(XMVectorGetX(matrix.r[0]), XMVectorGetX(matrix.r[1]),
		XMVectorGetX(matrix.r[2]), XMVectorGetX(matrix.r[3])); // _11, _21, _31, _41
	XMVECTOR col1 = XMVectorSet(XMVectorGetY(matrix.r[0]), XMVectorGetY(matrix.r[1]),
		XMVectorGetY(matrix.r[2]), XMVectorGetY(matrix.r[3])); // _12, _22, _32, _42
	XMVECTOR col2 = XMVectorSet(XMVectorGetZ(matrix.r[0]), XMVectorGetZ(matrix.r[1]),
		XMVectorGetZ(matrix.r[2]), XMVectorGetZ(matrix.r[3])); // _13, _23, _33, _43
	XMVECTOR col3 = XMVectorSet(XMVectorGetW(matrix.r[0]), XMVectorGetW(matrix.r[1]),
		XMVectorGetW(matrix.r[2]), XMVectorGetW(matrix.r[3])); // _14, _24, _34, _44

	// Calculate near plane of frustum.
	planes[0] = XMPlaneNormalize(XMVectorAdd(col3, col2));
	// Calculate far plane of frustum.
	planes[1] = XMPlaneNormalize(XMVectorSubtract(col3, col2));
	// Calculate left plane of frustum.
	planes[2] = XMPlaneNormalize(XMVectorAdd(col3, col0));
	// Calculate right plane of frustum.
	planes[3] = XMPlaneNormalize(XMVectorSubtract(col3, col0));
	// Calculate top plane of frustum.
	planes[4] = XMPlaneNormalize(XMVectorSubtract(col3, col1));
	// Calculate bottom plane of frustum.
	planes[5] = XMPlaneNormalize(XMVectorAdd(col3, col1));
}

bool DirectX::Frustum::CheckPoint(float x, float y, float z)
{
	// Check if the point is inside all six planes of the view frustum.
	for (int i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet(x, y, z, 1.0f))) < 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool DirectX::Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	// Check if any one point of the cube is in the view frustum.
	for (int i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter - radius), (yCenter - radius), (zCenter - radius), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter + radius), (yCenter - radius), (zCenter - radius), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter - radius), (yCenter + radius), (zCenter - radius), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter + radius), (yCenter + radius), (zCenter - radius), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter - radius), (yCenter - radius), (zCenter + radius), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter + radius), (yCenter - radius), (zCenter + radius), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter - radius), (yCenter + radius), (zCenter + radius), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter + radius), (yCenter + radius), (zCenter + radius), 1.0f))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

bool DirectX::Frustum::CheckSphere(const ObjectRef& object)
{
	// Performance optimization
	const Sphere& sphere = object->mesh->boundingSphere;
	const Vec3f& location = object->transform.location;
	const Vec3f& scale = object->transform.scale;

	const float xCenter = sphere.center.x + location.x;
	const float yCenter = sphere.center.y + location.y;
	const float zCenter = sphere.center.z + location.z;

	float scaleFactor = scale.x > scale.y ? scale.x : scale.y;
	scaleFactor = scaleFactor > scale.z ? scaleFactor : scale.z;

	return Frustum::CheckSphere(xCenter, yCenter, zCenter, sphere.radius * scaleFactor);
}

bool DirectX::Frustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	// Check if the radius of the sphere is inside the view frustum.
	for (int i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet(xCenter, yCenter, zCenter, 1.0f))) < -radius)
		{
			return false;
		}
	}

	return true;
}

bool DirectX::Frustum::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for (int i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter - xSize), (yCenter - ySize), (zCenter - zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter + xSize), (yCenter - ySize), (zCenter - zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter - xSize), (yCenter + ySize), (zCenter - zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter - xSize), (yCenter - ySize), (zCenter + zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter + xSize), (yCenter + ySize), (zCenter - zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter + xSize), (yCenter - ySize), (zCenter + zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter - xSize), (yCenter + ySize), (zCenter + zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(planes[i], XMVectorSet((xCenter + xSize), (yCenter + ySize), (zCenter + zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}
