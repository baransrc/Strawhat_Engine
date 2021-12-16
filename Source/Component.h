#pragma once

#include "ComponentType.h"

class Entity;

class Component
{
protected:
	bool enabled;
	Entity* owner;
public:
	Component();
	virtual ~Component() = default;

	virtual component_type Type();
	virtual void Initialize();
	virtual void Enable();
	virtual void Disable();
	virtual void Update();
	virtual bool IsEnabled();
};