#include "AssimpMeshLoader.h"

#include "../Core/CubeMesh.h"
#include "../Core/Logger.h"

Assimp::AssimpMeshLoader::AssimpMeshLoader(MeshReadFlag readFlag) : readFlag(readFlag)
{
}

MeshRef Assimp::AssimpMeshLoader::LoadMesh(str fileName)
{
	const MeshRef placeholderMesh = CubeMesh;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName, readFlag | aiProcess_GenNormals);

	if (scene == nullptr)
	{
		log_error("Assimp scene is not loaded from file \"{}\". {} mesh will be used as placeholder.", fileName, placeholderMesh->name);
		return placeholderMesh;
	}

	aiNode* node = scene->mRootNode;

	if (node->mNumMeshes <= 0)
	{
		log_warn("Assimp scene has no meshes. File: \"{}\". {} mesh will be used as placeholder.", fileName, placeholderMesh->name);
		return placeholderMesh;
	}

	aiMesh* rawMesh = scene->mMeshes[node->mMeshes[0]]; // We load only the first mesh for now

	if (rawMesh->mNumVertices <= 0)
	{
		log_warn("Assimp scene has no vertices. File: \"{}\". {} mesh will be used as placeholder.", fileName, placeholderMesh->name);
		return placeholderMesh;
	}

	MeshRef mesh = MakeRef<Mesh>();
	if (node->mName.length)
	{
		mesh->name = node->mName.data;
	}
	else
	{
		auto const startPos = fileName.find_last_of("/\\");
		auto const endPos = fileName.find_last_of(".");
		mesh->name = fileName.substr(startPos + 1, endPos - startPos - 1);
	}

	mesh->vertices.reserve(rawMesh->mNumVertices);

	for (u32 i = 0; i < rawMesh->mNumVertices; i++)
	{
		Vertex vertex{};

		vertex.position.x = rawMesh->mVertices[i].x;
		vertex.position.y = rawMesh->mVertices[i].y;
		vertex.position.z = rawMesh->mVertices[i].z;

		registerVertexPosition(vertex.position);

		if (rawMesh->HasNormals())
		{
			vertex.normal.x = rawMesh->mNormals[i].x;
			vertex.normal.y = rawMesh->mNormals[i].y;
			vertex.normal.z = rawMesh->mNormals[i].z;
		}

		if (rawMesh->mTextureCoords[0])
		{
			vertex.texCoord.x = (float)rawMesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)rawMesh->mTextureCoords[0][i].y;
		}

		mesh->vertices.push_back(vertex);
	}

	mesh->boundingSphere.center = calculateBoundingCenter();
	mesh->boundingSphere.radius = calculateBoundingRadius();
	resetBoundingBox();

	log_info("vertices read {}", mesh->vertices.size());

	for (u32 i = 0; i < rawMesh->mNumFaces; i++)
	{
		aiFace face = rawMesh->mFaces[i];

		for (u32 j = 0; j < face.mNumIndices; j++)
		{
			mesh->indices.push_back(face.mIndices[j]);
		}
	}

	log_info("indices read {}", mesh->indices.size());
	log_info("bounding sphere center {}", mesh->boundingSphere.center);
	log_info("bounding sphere radius {}", mesh->boundingSphere.radius);

	return mesh;
}
