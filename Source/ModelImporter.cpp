#include "ModelImporter.h"
#include "Entity.h"
#include "ComponentBoundingBox.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ASSIMP/scene.h"
#include "Globals.h"							// For LOG
#include "Util.h"								// For String functions
#include "Application.h"						// For Access to App
#include "ModuleTexture.h"						// For Access to Texture::Load and Texture::Unload
#include "MATH_GEO_LIB/Geometry/Polyhedron.h"	// For OBB::ToPolyhedron
#include "MATH_GEO_LIB/Geometry/Sphere.h"		// For OBB::ToMinimumEnclosingSphere
#include "assimp/postprocess.h"					// For aiProcess_Triangulate, aiProcess_FlipUVs
#include "assimp/Importer.hpp"					// For Assimp::Importer

namespace ModelImporter
{
	// Internal functions to be hidden from external usage:
	namespace
	{
		/// <summary>
		/// Loads texture in given file_path, if cannot load sucessfully, it unloads the default texture it loaded.
		/// </summary>
		/// <param name="output_texture_id">Texture id loaded from file_path</param>
		/// <param name="file_path">Path to the texture file.</param>
		/// <returns>True if loading was successful, false if not.</returns>
		bool ModelImporter_TryLoadingTextureFromFile(unsigned int& output_texture_id, const char* file_path)
		{
			output_texture_id = 0;
			bool successful = false;

			output_texture_id = App->texture->LoadTexture
			(
				file_path,
				GL_NEAREST,
				GL_NEAREST,
				GL_CLAMP_TO_EDGE,
				GL_CLAMP_TO_EDGE,
				true,
				true,
				successful
			);

			if (!successful)
			{
				App->texture->UnloadTexture(&output_texture_id);
			}

			return successful;
		}

		/// <summary>
		/// Searches and loads texture in three steps:
		/// 1. Inside the directory specified in the model file.
		/// 2. Inside the directory of model.
		/// 3. Inside the default texture file of the engine.
		/// </summary>
		/// <param name="output_texture_id">Id of loaded texture</param>
		/// <param name="path_to_texture">Path to texture</param>
		/// <param name="path_to_parent_directory">Path to model directory</param>
		/// <returns>True if loading was successful, false if not.</returns>
		bool ModelImporter_LoadTexture(unsigned int& output_texture_id, const char* path_to_texture, const char* path_to_parent_directory)
		{
			static const int search_in_specified_dir = 0;
			static const int search_in_model_dir = 1;
			static const int search_in_default_texture_dir = 2;
			static const int step_count = 3;

			bool successful = false;

			for (int trial = search_in_specified_dir; trial < step_count; ++trial)
			{
				switch (trial)
				{
					// Search inside the directory specified in the model file:
					case search_in_specified_dir: 
					{
						LOG("Searching for texture file specified in \"%s\"", path_to_texture);

						successful = ModelImporter_TryLoadingTextureFromFile(output_texture_id, path_to_texture);
					}
					break;

					// Search in the parent directory of the model:
					case search_in_model_dir:
					{
						char* texture_file_name = util::CopyCString(path_to_texture);
						util::SubstrBeforeCharFromEnd(&texture_file_name, '\\');
						char* path_in_model_dir = util::ConcatCStrings(path_to_parent_directory, texture_file_name);

						LOG("Searching for texture file specified in \"%s\"", path_in_model_dir);

						successful = ModelImporter_TryLoadingTextureFromFile(output_texture_id, path_in_model_dir);

						free(path_in_model_dir);
						free(texture_file_name);
					}
					break;

					// Search in the default texture directory of Strawhat Engine:
					case search_in_default_texture_dir:
					{
						char* texture_file_name = util::CopyCString(path_to_texture);
						util::SubstrBeforeCharFromEnd(&texture_file_name, '\\');
						char* default_texture_dir = util::ConcatCStrings(App->GetWorkingDirectory(), TEXTURES_FOLDER);
						char* path_in_default_texture_dir = util::ConcatCStrings(default_texture_dir, texture_file_name);

						LOG("Searching for texture file specified in \"%s\"", path_in_default_texture_dir);

						successful = ModelImporter_TryLoadingTextureFromFile(output_texture_id, path_in_default_texture_dir);

						free(path_in_default_texture_dir);
						free(default_texture_dir);
						free(texture_file_name);
					}
					break;
				}

				if (successful)
				{
					LOG("Search completed successfully.");
					break;
				}
				else
				{
					LOG("Search was unsuccessful.");
				}
			}

			return successful;
		}

		/// <summary>
		/// Loads textures and creates an array of texture ids.
		/// </summary>
		/// <param name="scene">Model</param>
		/// <param name="path_to_parent_directory">Path to model directory</param>
		/// <returns>Array of texture ids.</returns>
		unsigned int* ModelImporter_LoadTextureIds(const aiScene* scene, const char* path_to_parent_directory, const char* file_name)
		{
			//size_t number_of_textures = scene->mNumMaterials; // For now we assume we have one texture for each material.
			//TODO: How are we supposed to know how many texture materials has the fbx if it's broken
			size_t number_of_textures = 3; 

			// Allocate memory for all the textures in scene:
			unsigned int* texture_ids = (unsigned int*)calloc(number_of_textures, sizeof(unsigned int));

			// For storing file name of textures of each iteration of the below loop:
			aiString texture_file_path;

			char* diffuse = util::ConcatCStrings(file_name, "Diffuse.png");;
			char* specular = util::ConcatCStrings(file_name, "Specular.tif");;
			char* occlusion = util::ConcatCStrings(file_name, "Occlusion.png");;
			
			unsigned int texture_id = 0;
			bool loaded = ModelImporter_LoadTexture(texture_id, diffuse, path_to_parent_directory);
			if (loaded)
			{
				texture_ids[0] = texture_id;
			}
			loaded = ModelImporter_LoadTexture(texture_id, specular, path_to_parent_directory);
			if (loaded)
			{
				texture_ids[1] = texture_id;
			}
			loaded = ModelImporter_LoadTexture(texture_id, occlusion, path_to_parent_directory);
			if (loaded)
			{
				texture_ids[2] = texture_id;
			}

			//for (size_t i = 0; i < number_of_textures; ++i)
			//{
			//	// Get texture file name:
			//	/*aiReturn is_diffuse_texture = scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_file_path);
			//	std::string model_texture_path(texture_file_path.data);
			//	// If couldn't load the diffuse texture file name from material,
			//	// continue to the next material:
			//	if (is_diffuse_texture != aiReturn_SUCCESS)
			//	{
			//		continue;
			//	}*/

			//	unsigned int texture_id = i;
			//	bool loaded = ModelImporter_LoadTexture(texture_id, texture_file_path.C_Str(), path_to_parent_directory);

			//	// Since calloc defaults to 0, don't assign if not successful:
			//	// TODO: Make sure texture with id 0 is the default texture.
			//	if (loaded)
			//	{
			//		texture_ids[i] = texture_id;
			//	}
			//}

			return texture_ids;
		}

		ComponentMesh* ModelImporter_LoadComponentMeshFromMeshData(aiMesh* mesh_data, Entity* owner, unsigned int* texture_ids, size_t number_of_textures)
		{
			size_t number_of_vertices = mesh_data->mNumVertices;
			size_t number_of_triangles = mesh_data->mNumFaces;

			float* vertices = (float*)calloc(number_of_vertices * 8, sizeof(float)); // Initializes to 0 by default.

			for (size_t i = 0; i < number_of_vertices * 8; i += 8)
			{
				const aiVector3D position = mesh_data->mVertices[i / 8];
				const aiVector3D normal = mesh_data->mNormals[i / 8];

				// Position:
				vertices[i + 0] = position.x; // NOTE: Yeah I'm adding +0 because I like consistency, please don't judge me.
				vertices[i + 1] = position.y;
				vertices[i + 2] = position.z;

				// Normal:
				vertices[i + 3] = normal.x;
				vertices[i + 4] = normal.y;
				vertices[i + 5] = normal.z;

				// Since all the vertices data is initialized to 0.0f by default,
				// if mesh_data->mTextureCoords[0] is null, leave texture coordinates 
				// as zero, and move to the next iteration.
				if (!mesh_data->mTextureCoords[0])
				{
					continue;
				}

				const aiVector3D texture_coordinate = mesh_data->mTextureCoords[0][i / 8];

				// Texture Coordinates:
				vertices[i + 6] = texture_coordinate.x;
				vertices[i + 7] = texture_coordinate.y;
			}

			size_t number_of_faces = mesh_data->mNumFaces;
			size_t number_of_indices = number_of_faces * 3; // Assuming there are 3 vertices per triangle.

			unsigned int* indices = (unsigned int*)calloc(number_of_indices, sizeof(unsigned int)); // Initializes to 0 by default.

			for (size_t i = 0; i < number_of_faces; ++i)
			{
				indices[i * 3 + 0] = mesh_data->mFaces[i].mIndices[0]; // NOTE: It's me again, adding 0 for the pure sake of visual consistency.
				indices[i * 3 + 1] = mesh_data->mFaces[i].mIndices[1];
				indices[i * 3 + 2] = mesh_data->mFaces[i].mIndices[2];
			}

			// NOTE: This function passes dynamically allocated vertices and indices arrays to the created component_mesh.
			// component_mesh is responsible for the deallocation of those resources.
			
			ComponentMesh* component_mesh = new ComponentMesh();
			component_mesh->Initialize(owner);
			component_mesh->Load(vertices, indices, texture_ids, number_of_vertices, number_of_indices, number_of_triangles, number_of_textures);

			return component_mesh;
		}
		
		/// <summary>
		/// Loads meshes as entity with child entities having mesh components.
		/// </summary>
		/// <param name="scene">Model</param>
		/// <param name="scene_name">Name of model</param>
		/// <param name="texture_ids">An array consisting of texture ids</param>
		/// <returns>Entity with child entities having mesh components.</returns>
		Entity* ModelImporter_LoadMeshesAsEntity(const aiScene* scene, const char* scene_name, unsigned int* texture_ids)
		{
			Entity* model_entity = new Entity();
			model_entity->Initialize(scene_name);

			ComponentBoundingBox* bounding_box = new ComponentBoundingBox();
			bounding_box->Initialize(model_entity);

			size_t number_of_meshes = scene->mNumMeshes;
			//size_t number_of_textures = scene->mNumMaterials; // For now we assume we have one texture for each material.
			size_t number_of_textures = 3; // For now we assume we have three texture for each material.

			// For logging purposes:
			size_t number_of_loaded_meshes = 0;
			size_t number_of_triangles = 0;
			size_t number_of_indices = 0;
			size_t number_of_vertices = 0;

			LOG("Loading model as entity named %s", scene_name);

			aiMesh* current_mesh_data;

			for (size_t i = 0; i < number_of_meshes; ++i)
			{
				current_mesh_data = scene->mMeshes[i];

				// Get Texture IDs:
				//unsigned int* texture_ids = ModelImporter_LoadTextureIds(model, path_to_parent_directory, name_of_file);

				Entity* current_node = new Entity();
				current_node->Initialize((current_mesh_data->mName.C_Str()));
				current_node->SetParent(model_entity);

				ComponentBoundingBox* component_bounding_box = new ComponentBoundingBox();

				component_bounding_box->Initialize(current_node);

				ComponentMaterial* component_material = new ComponentMaterial();
				component_material->Initialize(current_node);
				component_material->Load(texture_ids, number_of_textures);


				ComponentMesh* current_component_mesh = 
					ModelImporter_LoadComponentMeshFromMeshData(current_mesh_data, current_node, texture_ids, number_of_textures);

				// For logging purposes:
				number_of_triangles += current_component_mesh->GetNumberOfTriangles();
				number_of_indices += current_component_mesh->GetNumberOfIndices();
				number_of_vertices += current_component_mesh->GetNumberOfVertices();
				++number_of_loaded_meshes;
			}

			LOG("Loaded model as entity named %s:\n\tNumber of child meshes: %zu\n\tNumber of triangles: %zu\n\tNumber of indices: %zu\n\tNumber of vertices: %zu",
				scene_name,
				number_of_loaded_meshes,
				number_of_triangles,
				number_of_indices,
				number_of_vertices
			);

			return model_entity;
		}
	}

	/// <summary>
	/// Loads Entity with children entities having ComponentMesh from the given model file.
	/// </summary>
	/// <param name="path_to_file">File path of the model</param>
	/// <returns>Model as entity. User is responsible for deletion.</returns>
	Entity* Import(const char* path_to_file)
	{
		Assimp::Importer importer;
		const aiScene* model = importer.ReadFile(path_to_file, aiProcess_Triangulate | aiProcess_FlipUVs);

		// aiProcess_Triangulate: If the model is not entirely consisting of triangles, transform all the
		// primitive to triangles.
		// aiProcess_FlipUVs: If texture image is reversed around the y-axis, flip it.
		// Can use aiProcess_GenNormals as well in the future to create normal vectors for each vertex if 
		// the loaded model has no vertex normal data.
		// NOTE: For more flags check http://assimp.sourceforge.net/lib_html/postprocess_8h.html

		if (!model)
		{
			LOG("Error Loading Model File \"%s\": %s", path_to_file, importer.GetErrorString());
			return nullptr;
		}
		else
		{
			LOG("Model file \"%s\" is loaded successfully.", path_to_file);
		}

		// Get the parent directory path from full file path:
		char* path_to_parent_directory = util::ConcatCStrings("", path_to_file);
		util::SubstrAfterCharFromEnd(&path_to_parent_directory, '\\');

		//Get only the name of the file
		char* name_of_file = util::ConcatCStrings("", path_to_file);
		name_of_file = util::GetStringBetween(name_of_file, "\\", ".");
		util::SubstrBeforeCharFromEnd(&name_of_file, '\\');

		// Get Texture IDs:
		unsigned int* texture_ids = ModelImporter_LoadTextureIds(model, path_to_parent_directory, name_of_file);

		char* model_name = util::ConcatCStrings("", path_to_file);
		util::SubstrBeforeCharFromEnd(&model_name, '\\');

		Entity* loaded_model = ModelImporter_LoadMeshesAsEntity(
			model,
			model_name,
			texture_ids
		);

		// Deallocate resources:
		free(model_name);
		free(texture_ids);
		free(path_to_parent_directory);

		return loaded_model;
	}
}