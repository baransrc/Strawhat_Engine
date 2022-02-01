#include "QuadTree.h"

#include "Entity.h"
#include "ComponentMesh.h"
#include "ComponentBoundingBox.h"

QuadTreeNode::QuadTreeNode()
{
	children[0] = nullptr;
	children[1] = nullptr;
	children[2] = nullptr;
	children[3] = nullptr;
}

QuadTreeNode::~QuadTreeNode()
{
}

bool QuadTreeNode::IsLeaf() const
{
	// Since quadtrees either contain 4 children node, 
	// or they are a leaf, checking for the first child
	// node suffices to say if this node is the LeafNode.

	return children[0] == nullptr;
}

QuadTreeNode* const QuadTreeNode::GetParent() const
{
	return parent;
}

QuadTreeNode* QuadTreeNode::GetChildren() const
{
	return *children;
}

const math::AABB& QuadTreeNode::GetContainer() const
{
	return container;
}

void QuadTreeNode::SetChild(size_t index, QuadTreeNode* new_child)
{
	// Give error if the index is bigger than 3:
	assert(index <= 3);

	children[index] = new_child;
}

void QuadTreeNode::SetParent(QuadTreeNode* new_parent)
{
	parent = new_parent;
}

void QuadTreeNode::Insert(Entity* const entity) const
{
}

void QuadTreeNode::Remove(Entity* const entity) const
{
}

void QuadTreeNode::CollectEntities(std::vector<Entity*>& entities) const
{
}

void QuadTreeNode::CollectMeshComponents(std::vector<ComponentMesh*>& mesh_components) const
{
}

void QuadTreeNode::DistributeChildren()
{
}
