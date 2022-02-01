#pragma once

#include "MATH_GEO_LIB/Geometry/AABB.h"

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
	const math::AABB& GetContainer() const;
	void SetChild(size_t index, QuadTreeNode* new_child);
	void SetParent(QuadTreeNode* new_parent);
	void Insert(Entity* const entity) const;
	void Remove(Entity* const entity) const;
	void CollectEntities(std::vector<Entity*>& entities) const;
	void CollectMeshComponents(std::vector<ComponentMesh*>& mesh_components) const;
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
