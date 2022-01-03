#include "ModelImporter.h"
#include "Entity.h"
#include "ComponentMesh.h"
#include "ASSIMP/scene.h"
#include "Globals.h"							// For LOG
#include "Util.h"								// For String functions
#include "Application.h"						// For Access to App
#include "ModuleTexture.h"						// For Access to Texture::Load and Texture::Unload
#include "MATH_GEO_LIB/Geometry/Polyhedron.h"	// For OBB::ToPolyhedron
#include "MATH_GEO_LIB/Geometry/Sphere.h"		// For OBB::ToMinimumEnclosingSphere
#include "assimp/postprocess.h"					// For aiProcess_Triangulate, aiProcess_FlipUVs
#include "assimp/Importer.hpp"					// For Assimp::Importer

/// <summary>
/// Loads Entity with children entities having ComponentMesh from the given model file.
/// </summary>
/// <param name="path_to_file">File path of the model</param>
/// <returns>Model as entity. User is responsible for deletion.</returns>
Entity* ModelImporter::Import(const char* path_to_file)
{
	Assimp::Importer importer;

	char* path_to_parent_directory = util::ConcatCStrings("", path_to_file);
	util::SubstrAfterCharFromEnd(&path_to_parent_directory, '\\');


	const aiScene* scene = importer.ReadFile(path_to_file, aiProcess_Triangulate | aiProcess_FlipUVs);

	// aiProcess_Triangulate: If the model is not entirely consisting of triangles, transform all the
	// primitive to triangles.
	// aiProcess_FlipUVs: If texture image is reversed around the y-axis, flip it.
	// Can use aiProcess_GenNormals as well in the future to create normal vectors for each vertex if 
	// the loaded model has no vertex normal data.
	// NOTE: For more flags check http://assimp.sourceforge.net/lib_html/postprocess_8h.html

	if (!scene)
	{
		LOG("Error Loading Model File \"%s\": %s", path_to_file, importer.GetErrorString());
		return nullptr;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Start Loading Textures
	//
	size_t number_of_textures = scene->mNumMaterials; // For now we assume we have one texture for each material.
	size_t number_of_loaded_textures = 0;

	// Allocate memory for all the textures in scene:
	unsigned int* texture_ids = (unsigned int*)calloc(number_of_textures, sizeof(unsigned int));

	// For storing file name of textures of each iteration of the below loop:
	aiString texture_file_path;

	for (size_t i = 0; i < scene->mNumMaterials; ++i)
	{
		// Get texture file name:
		aiReturn is_diffuse_texture = scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_file_path);

		// If couldn't load the diffuse texture file name from material,
		// continue to the next material:
		if (is_diffuse_texture != aiReturn_SUCCESS)
		{
			continue;
		}

		bool succesfully_loaded = true;

		LOG("Loading texture for material number %i", i);

		LOG("Searching for texture file specified in \"%s\"", texture_file_path.C_Str());

		// Try loading from the path given in model file:
		texture_ids[number_of_loaded_textures] = App->texture->LoadTexture
		(
			texture_file_path.C_Str(),
			GL_NEAREST,
			GL_NEAREST,
			GL_CLAMP_TO_EDGE,
			GL_CLAMP_TO_EDGE,
			true,
			true,
			succesfully_loaded
		);

		if (succesfully_loaded)
		{
			LOG("Successfully loaded texture in file\"%s\"", texture_file_path.C_Str());
		}
		// In case of failure in finding the texture file in the path given in the model file,
		// Take the name of the texture file and search for it inside the directory of the 
		// model file:
		else
		{
			LOG("Could not find the texture file specified in \"%s\"", texture_file_path.C_Str());

			// Store texture file path as char*:
			char* temp_texture_file_name = util::ConcatCStrings("", texture_file_path.C_Str());

			util::SubstrBeforeCharFromEnd(&temp_texture_file_name, '\\');

			// Form path of texture next to model file:
			aiString file_in_model_directory(path_to_parent_directory);
			file_in_model_directory.Append(temp_texture_file_name);

			LOG("Searching for texture file specified in \"%s\"", file_in_model_directory.C_Str());

			// Unload previously loaded error texture:
			App->texture->UnloadTexture(&texture_ids[number_of_loaded_textures]);
			// Load texture with new parameters:
			texture_ids[number_of_loaded_textures] = App->texture->LoadTexture
			(
				file_in_model_directory.C_Str(),
				GL_NEAREST,
				GL_NEAREST,
				GL_CLAMP_TO_EDGE,
				GL_CLAMP_TO_EDGE,
				true,
				true,
				succesfully_loaded
			);

			if (succesfully_loaded)
			{
				LOG("Successfully loaded texture in file\"%s\"", file_in_model_directory.C_Str());
			}
			else // If texture still remains unable to be loaded, try to find it inside the texture folder.
			{
				LOG("Could not find the texture file specified in \"%s\"", file_in_model_directory.C_Str());

				aiString file_in_working_directory(App->GetWorkingDirectory());
				file_in_working_directory.Append(TEXTURES_FOLDER);
				file_in_working_directory.Append(temp_texture_file_name);

				LOG("Searching for texture file specified in \"%s\"", file_in_working_directory.C_Str());

				// Unload previously loaded error texture:
				App->texture->UnloadTexture(&texture_ids[number_of_loaded_textures]);
				// Load texture with new parameters:
				texture_ids[number_of_loaded_textures] = App->texture->LoadTexture
				(
					file_in_working_directory.C_Str(),
					GL_NEAREST,
					GL_NEAREST,
					GL_CLAMP_TO_EDGE,
					GL_CLAMP_TO_EDGE,
					true,
					true,
					succesfully_loaded
				);

				if (succesfully_loaded)
				{
					LOG("Successfully loaded texture in file\"%s\"", file_in_working_directory.C_Str());
				}
				else
				{
					// Unload previously loaded error texture:
					App->texture->UnloadTexture(&texture_ids[number_of_loaded_textures]);

					LOG("Could not find the texture file specified in \"%s\"", file_in_working_directory.C_Str());
					LOG("Aborting texture file lookup, model will be loaded without textures");
				}
			}

			// Delete temp_texture_file_name as it's not needed anymore:
			free(temp_texture_file_name);
		}

		++number_of_loaded_textures;
	}
	//
	// End Loading Textures
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Start Loading Meshes
	//
	Entity* model_entity = new Entity();
	model_entity->Initialize("Fake Entity");

	size_t number_of_meshes = scene->mNumMeshes;
	size_t number_of_loaded_meshes = 0;
	
	size_t number_of_triangles = 0;
	size_t number_of_indices = 0;
	size_t number_of_vertices = 0;

	aiMesh* current_mesh_data;

	for (size_t i = 0; i < number_of_meshes; ++i)
	{
		current_mesh_data = scene->mMeshes[i];

		ComponentMesh* current_component_mesh = new ComponentMesh();

		Entity* current_node = new Entity();
		current_node->Initialize(current_mesh_data->mName.C_Str());
		current_node->SetParent(model_entity);

		current_component_mesh->Initialize(current_node);
		current_component_mesh->Load(current_mesh_data, texture_ids, number_of_textures);

		number_of_triangles += current_component_mesh->GetNumberOfTriangles();
		number_of_indices += current_component_mesh->GetNumberOfIndices();
		number_of_vertices += current_component_mesh->GetNumberOfVertices();

		++number_of_loaded_meshes;
	}
	//
	// End Loading Meshes
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Delete resources:
	free(texture_ids);
	free(path_to_parent_directory);

	return model_entity;
}