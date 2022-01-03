#pragma once

class Entity;

class Importer
{
public:
	virtual ~Importer() = default;

	virtual Entity* Import(const char* file_path) = 0;
	virtual void Clear() = 0;
};