#include "MeshLoader.h"

#include <cmath>

using namespace Core;

void MeshLoader::registerVertexPosition(const Vec3f& position)
{
    if (position.x > maxX) maxX = position.x;
    if (position.x < minX) minX = position.x;
    if (position.y > maxY) maxY = position.y;
    if (position.y < minY) minY = position.y;
    if (position.z > maxZ) maxZ = position.z;
    if (position.z < minZ) minZ = position.z;
}

Vec3f MeshLoader::calculateBoundingCenter() const
{
    return {
        .x = minX + (maxX - minX) / 2,
        .y = minY + (maxY - minY) / 2,
        .z = minZ + (maxZ - minZ) / 2,
    };
}

float MeshLoader::calculateBoundingRadius() const
{
    float a = maxX - minX;
    float b = maxY - minY;
    float c = maxZ - minZ;
    return std::sqrtf(a * a + b * b + c * c) / 2;
}

void MeshLoader::resetBoundingBox()
{
    minX = FLT_MAX;
    maxX = FLT_MIN;
    minY = FLT_MAX;
    maxY = FLT_MIN;
    minZ = FLT_MAX;
    maxZ = FLT_MIN;
}
