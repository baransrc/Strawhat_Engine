#pragma once

#include "ComponentType.h"

class Entity;

class Component
{
private:
	/// <summary>
	/// Unique id of this Component
	/// </summary>
	unsigned int id;

protected:
	/// <summary> 
	/// Is this Component enabled or not?
	/// </summary>
	bool enabled;

	/// <summary>
	/// Owner Entity of this Component
	/// </summary>
	Entity* owner;

public:
	/// <summary>
	/// Default constructor for Component.
	/// As this sets the unique id of this component, 
	/// this must be called inside the default constructor
	/// of derived components
	/// </summary>
	Component();

	virtual ~Component() = default;

	/// <returns>
	/// Owner of this Component.
	/// </returns>
	Entity* Owner() const;
	
	/// <returns>
	/// Type of this Component.
	/// </returns>
	virtual component_type Type() const;

	/// <returns> 
	/// If the component is enabled.
	/// </returns>
	bool Enabled() const;

	/// <returns>
	/// Unique id of this Component.
	/// </returns>
	unsigned int Id() const;
	
	/// <summary>
	/// Is the type of Component can appear more than one in an Entity?
	/// </summary>
	/// <param name="type">Type of the Component</param>
	/// <returns>true if there can be more than one of Component of the same type in Entity, false if not.</returns>
	static bool CanBeMoreThanOne(component_type type);
	
	/// <summary>
	/// Initializes the component with it's owner.
	/// </summary>
	/// <param name="new_owner">Entity to be this Component's owner</param>
	virtual void Initialize(Entity* new_owner);

	/// <summary>
	/// Enables the component.
	/// </summary>
	virtual void Enable();

	/// <summary>
	/// Disables the component.
	/// </summary>
	virtual void Disable();

	/// <summary>
	/// Called on each pre-update of the owner Entity.
	/// </summary>
	virtual void PreUpdate();

	/// <summary>
	/// Called on each Update of the owner Entity.
	/// </summary>
	virtual void Update();

	/// <summary>
	/// Called on each PostUpdate of the owner Entity.
	/// </summary>
	virtual void PostUpdate();

	/// <summary>
	/// Draws Gizmo of the component in editor mode.
	/// </summary>
	virtual void DrawGizmo();

	/// <summary>
	/// Draws Component related info to Inspector in editor mode.
	/// </summary>
	void DrawInspector();

protected:
	/// <summary>
	/// Draws Inspector content of the component in editor mode.
	/// Use this function to draw custom content inside Inspector.
	/// To draw anything, you must use ImGui widgets, otherwise, you
	/// may get weird results.
	/// </summary>
	virtual void DrawInspectorContent();

private:
	/// <summary>
	/// Used for giving an id to the Component in Initialize method.
	/// </summary>
	/// <returns>A runtime unique id to the caller</returns>
	unsigned int GetCurrentId();
};