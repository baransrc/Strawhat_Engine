#pragma once

class Entity;

namespace ModelImporter
{
    Entity* Import(const char* file_path);
};