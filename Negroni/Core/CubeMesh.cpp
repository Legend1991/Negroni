#include "CubeMesh.h"
#include "ID.h"

#include <random>

namespace Core
{
    static std::random_device randomDevice;
    static std::mt19937 engine(randomDevice());
    static std::uniform_int_distribution<uint32_t> uniformDistribution(1, 16777215);

    MeshRef CubeMesh = std::shared_ptr<Mesh>(new Mesh{
        .id = ID(uniformDistribution(engine)),
        .name = "Cube",
        .vertices = {
            { .position = { -0.5f, 1.0f, -0.5f }, .normal {0.0f, 1.0f, 0.0f}, .texCoord = { 1.0f, 0.0f }},
            { .position = {  0.5f, 1.0f, -0.5f }, .normal {0.0f, 1.0f, 0.0f}, .texCoord = { 0.0f, 0.0f }},
            { .position = {  0.5f, 1.0f,  0.5f }, .normal {0.0f, 1.0f, 0.0f}, .texCoord = { 0.0f, 1.0f }},
            { .position = { -0.5f, 1.0f,  0.5f }, .normal {0.0f, 1.0f, 0.0f}, .texCoord = { 1.0f, 1.0f }},

            { .position = { -0.5f, 0.0f, -0.5f }, .normal {0.0f, -1.0f, 0.0f}, .texCoord = { 0.0f, 0.0f }},
            { .position = {  0.5f, 0.0f, -0.5f }, .normal {0.0f, -1.0f, 0.0f}, .texCoord = { 1.0f, 0.0f }},
            { .position = {  0.5f, 0.0f,  0.5f }, .normal {0.0f, -1.0f, 0.0f}, .texCoord = { 1.0f, 1.0f }},
            { .position = { -0.5f, 0.0f,  0.5f }, .normal {0.0f, -1.0f, 0.0f}, .texCoord = { 0.0f, 1.0f }},

            { .position = { -0.5f, 0.0f,  0.5f }, .normal {-1.0f, 0.0f, 0.0f}, .texCoord = { 0.0f, 1.0f }},
            { .position = { -0.5f, 0.0f, -0.5f }, .normal {-1.0f, 0.0f, 0.0f}, .texCoord = { 1.0f, 1.0f }},
            { .position = { -0.5f, 1.0f, -0.5f }, .normal {-1.0f, 0.0f, 0.0f}, .texCoord = { 1.0f, 0.0f }},
            { .position = { -0.5f, 1.0f,  0.5f }, .normal {-1.0f, 0.0f, 0.0f}, .texCoord = { 0.0f, 0.0f }},

            { .position = {  0.5f, 0.0f,  0.5f }, .normal {1.0f, 0.0f, 0.0f}, .texCoord = { 1.0f, 1.0f }},
            { .position = {  0.5f, 0.0f, -0.5f }, .normal {1.0f, 0.0f, 0.0f}, .texCoord = { 0.0f, 1.0f }},
            { .position = {  0.5f, 1.0f, -0.5f }, .normal {1.0f, 0.0f, 0.0f}, .texCoord = { 0.0f, 0.0f }},
            { .position = {  0.5f, 1.0f,  0.5f }, .normal {1.0f, 0.0f, 0.0f}, .texCoord = { 1.0f, 0.0f }},

            { .position = { -0.5f, 0.0f, -0.5f }, .normal {0.0f, 0.0f, -1.0f}, .texCoord = { 0.0f, 1.0f }},
            { .position = {  0.5f, 0.0f, -0.5f }, .normal {0.0f, 0.0f, -1.0f}, .texCoord = { 1.0f, 1.0f }},
            { .position = {  0.5f, 1.0f, -0.5f }, .normal {0.0f, 0.0f, -1.0f}, .texCoord = { 1.0f, 0.0f }},
            { .position = { -0.5f, 1.0f, -0.5f }, .normal {0.0f, 0.0f, -1.0f}, .texCoord = { 0.0f, 0.0f }},

            { .position = { -0.5f, 0.0f,  0.5f }, .normal {0.0f, 0.0f, 1.0f}, .texCoord = { 1.0f, 1.0f }},
            { .position = {  0.5f, 0.0f,  0.5f }, .normal {0.0f, 0.0f, 1.0f}, .texCoord = { 0.0f, 1.0f }},
            { .position = {  0.5f, 1.0f,  0.5f }, .normal {0.0f, 0.0f, 1.0f}, .texCoord = { 0.0f, 0.0f }},
            { .position = { -0.5f, 1.0f,  0.5f }, .normal {0.0f, 0.0f, 1.0f}, .texCoord = { 1.0f, 0.0f }},
        },
        .indices = {
            3, 1, 0,
            2, 1, 3,

            6, 4, 5,
            7, 4, 6,

            11, 9, 8,
            10, 9, 11,

            14, 12, 13,
            15, 12, 14,

            19, 17, 16,
            18, 17, 19,

            22, 20, 21,
            23, 20, 22
        },
        .boundingSphere = { .radius = 1.7320508f }
    });
}
