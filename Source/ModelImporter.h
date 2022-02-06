#pragma once

class Entity;

namespace ModelImporter
{
	/// <summary>
	/// Loads Entity with children entities having ComponentMesh from the given model file.
	/// </summary>
	/// <param name="path_to_file">File path of the model</param>
	/// <returns>Model as entity. User is responsible for deletion.</returns>
    Entity* Import(const char* file_path);
};