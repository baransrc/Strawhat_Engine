#pragma once

#include "MATH_GEO_LIB/Geometry/AABB.h"
#include "MATH_GEO_LIB/Math/float3.h"

#include <list>

class Entity;
class ComponentMesh;

class QuadTreeNode
{
public:
	QuadTreeNode();
	~QuadTreeNode();

	bool IsLeaf() const;
	QuadTreeNode* const GetParent() const;
	QuadTreeNode* GetChildren() const;
	void CreateChildren();
	const math::AABB& GetContainer() const;
	void SetContainer(const math::AABB& new_container);
	void SetChild(size_t index, QuadTreeNode* new_child);
	void SetParent(QuadTreeNode* new_parent);
	void Insert(Entity* const entity);
	void Remove(Entity* const entity);
	void DistributeChildren();

private:
	QuadTreeNode* parent;
	QuadTreeNode* children[4];
	std::list<Entity*> entities;
	math::AABB container;
};

class QuadTree
{

};
