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

	Entity* Owner() const;
	bool Enabled() const;
	unsigned int Id() const;
	virtual component_type Type() const;
	static bool CanBeMoreThanOne(component_type type);
	virtual void Initialize(Entity* new_owner);
	virtual void Enable();
	virtual void Disable();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();
	virtual void DrawGizmo();
	void DrawInspector();

protected:
	virtual void DrawInspectorContent();

private:
	unsigned int GetCurrentId();
};