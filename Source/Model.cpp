#include "Model.h"
#include "Mesh.h"
#include "Globals.h" // For LOG
#include "Util.h" // For String functions
#include "Application.h" // For Access to App
#include "ModuleTexture.h" // For Access to Texture::Load and Texture::Unload
#include "assimp/postprocess.h"  // For aiProcess_Triangulate, aiProcess_FlipUVs
#include "assimp/Importer.hpp" // For Assimp::Importer

Model::Model()
{
}

Model::~Model()
{
	// Delete path_to_file:
	free(path_to_file);
	// Delete path_to_parent_directory:
	free(path_to_parent_directory);

	// Unload loaded textures:
	for (size_t i = 0; i < number_of_loaded_textures; ++i)
	{
		App->texture->UnloadTexture(&texture_ids[i]);
	}
	// Free memory occupied by texture_ids:
	free(texture_ids);
	texture_ids = nullptr;

	// Delete Meshes inside meshes:
	for (size_t i = 0; i < number_of_loaded_meshes; ++i)
	{
		delete meshes[i];
	}
	// Free memory occupied by meshes:
	free(meshes);
	meshes = nullptr;
}

/// <summary>
/// Draws all the meshes inside this Model.
/// </summary>
void Model::Draw() const
{
	// Draw all the meshes inside this model:
	for (size_t i = 0; i < number_of_loaded_meshes; ++i)
	{
		meshes[i]->Draw(texture_ids);
	}
}

/// <summary>
/// Loads the model with path 'new_path_to_file'.
/// </summary>
/// <param name="new_path_to_file"></param>
void Model::Load(const char* new_path_to_file)
{
	Assimp::Importer importer;

	LoadPathVariables(new_path_to_file);
	
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
		return;
	}

	// Load data related to mesh:
	LoadTextures(scene);
	LoadMeshes(scene);
}

/// <summary>
/// Loads path_to_file and path_to_parent_directory.
/// </summary>
/// <param name="new_path_to_file"></param>
void Model::LoadPathVariables(const char* new_path_to_file)
{
	path_to_file = util::ConcatCStrings("", new_path_to_file);
	
	path_to_parent_directory = util::ConcatCStrings("", path_to_file);
	util::SubstrAfterCharFromEnd(&path_to_parent_directory, '\\');
}

/// <summary>
/// Loads Textures specified in the model file.
/// For each texture of the file, firstly this, method checks 
/// if the specified texture exists in the path given by model file.
/// Secondly, it takes the file name out of specified path, and searches
/// for the file inside the directory of the model file. If all fails,
/// it searches for the texture inside Strawhat Engine's Texture folder.
/// </summary>
/// <param name="scene"></param>
void Model::LoadTextures(const aiScene* scene)
{
	number_of_textures = scene->mNumMaterials; // For now we assume we have one texture for each material.
	number_of_loaded_textures = 0;

	// Allocate memory for all the textures in scene:
	texture_ids = (unsigned int*)calloc(number_of_textures, sizeof(unsigned int));

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

		// In case of failure in finding the texture file in the path given in the model file,
		// Take the name of the texture file and search for it inside the directory of the 
		// model file:
		if (!succesfully_loaded)
		{
			// TODO: LOG.
			
			// Store texture file path as char*:
			char* temp_texture_file_name = util::ConcatCStrings("", texture_file_path.C_Str());

			util::SubstrBeforeCharFromEnd(&temp_texture_file_name, '\\');

			// Form path of texture next to model file:
			aiString file_in_model_directory(path_to_parent_directory);
			file_in_model_directory.Append(temp_texture_file_name);

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

			// If texture still remains unable to be loaded, try to find it inside the texture folder.
			if (!succesfully_loaded)
			{
				// TODO: LOG.

				aiString file_in_working_directory(App->GetWorkingDirectory());
				file_in_working_directory.Append(TEXTURES_FOLDER);
				file_in_working_directory.Append(temp_texture_file_name);

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
			}

			if (!succesfully_loaded)
			{
				// Unload previously loaded error texture:
				App->texture->UnloadTexture(&texture_ids[number_of_loaded_textures]);

				// TODO: LOG.
			}
			
			// Delete temp_texture_file_name as it's not needed anymore:
			free(temp_texture_file_name);
		}
		
		++number_of_loaded_textures;
	}
}

/// <summary>
/// Parses mesh data of the meshes inside scene, creates Mesh
/// for each of them and stores all the newly created Meshes 
/// inside meshes array.
/// </summary>
/// <param name="scene"></param>
void Model::LoadMeshes(const aiScene* scene)
{
	number_of_meshes = scene->mNumMeshes;
	number_of_loaded_meshes = 0;

	meshes = (Mesh**)calloc(number_of_meshes, sizeof(Mesh*));

	aiMesh* current_mesh_data;

	for (size_t i = 0; i < number_of_meshes; ++i)
	{
		current_mesh_data = scene->mMeshes[i];

		Mesh* current_mesh = new Mesh();

		current_mesh->Load(current_mesh_data);

		meshes[number_of_loaded_meshes] = current_mesh;

		++number_of_loaded_meshes;
	}
}