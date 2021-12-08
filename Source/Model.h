#pragma once
#include "ASSIMP/scene.h"
#include "MATH_GEO_LIB/Geometry/OBB.h"

class Mesh;

class Model
{
private:
	char* path_to_parent_directory;
	char* path_to_file;
	
	unsigned int* texture_ids;
	Mesh** meshes;

	math::OBB obb;
	float minimal_enclosing_sphere_radius;

	size_t number_of_textures;			// Stores the number of textures provided by Assimp.
	size_t number_of_loaded_textures;	// Stores the number of textures loaded successfully and added to texture_ids.
	size_t number_of_meshes;			// Stores the number of meshes provided by Assimp.
	size_t number_of_loaded_meshes;		// Stores the number of meshes could be parsed and added to meshes.

public:
	Model();
	~Model();

	bool Load(const char* new_path_to_file);
	void Draw() const;

	const math::OBB* GetOBB() const { return &obb; };
	math::float3 GetCenterPosition() const { return obb.CenterPoint(); };
	float GetMinimalEnclosingSphereRadius() const { return minimal_enclosing_sphere_radius; };

private:
	void LoadPathVariables(const char* new_path_to_file);
	void LoadTextures(const aiScene* scene);
	void LoadMeshes(const aiScene* scene);
	void LoadOBB();
	void DrawOBB() const;
};