#pragma once

#include "ComponentType.h"

class Entity;

class Component
{
private:
	unsigned int id;

protected:
	bool enabled;
	Entity* owner;


public:
	Component();
	virtual ~Component() = default;

	const Entity* Owner() const;
	bool Enabled() const;
	unsigned int Id() const;
	virtual component_type Type();
	virtual void Initialize(Entity* new_owner);
	virtual void Enable();
	virtual void Disable();
	virtual void Update();
	virtual void DrawGizmo();
	void DrawInspector();

protected:
	virtual void DrawInspectorContent();

private:
	unsigned int GetCurrentId();
};