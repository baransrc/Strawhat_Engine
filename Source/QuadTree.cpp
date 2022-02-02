#include "QuadTree.h"

#include "Entity.h"
#include "ComponentMesh.h"
#include "ComponentBoundingBox.h"

namespace StrawMath
{
	// NW NE
	// SW SE

#define NE 0
#define SE 1
#define SW 2
#define NW 3

#define QUADTREE_MAX_ITEMS 8
#define QUADTREE_MIN_SIZE 10.0f 

	QuadTreeNode::QuadTreeNode()
	{
		parent = nullptr;
		children[NE] = nullptr;
		children[SE] = nullptr;
		children[SW] = nullptr;
		children[NW] = nullptr;
	}

	QuadTreeNode::~QuadTreeNode()
	{
		for (size_t i = 0; i < 4; ++i)
		{
			if (children[i] != nullptr)
			{
				delete children[i];
			}
		}
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

	void QuadTreeNode::CreateChildren()
	{
		// We need to subdivide this node ...
		math::float3 size = container.Size();
		// We don't change it's y coordinate since this is a QuadTree,
		// not an Octree:
		math::float3 new_size(size.x * 0.5f, size.y, size.z * 0.5f);
		// We will calculate each new child node's center using 
		// their parent's center:
		math::float3 center = container.CenterPoint();
		math::float3 new_center = center;

		// Create North East node from center and size:
		new_center.x = center.x + size.x * 0.25f;
		new_center.z = center.z + size.z * 0.25f;
		children[NE] = new QuadTreeNode();
		children[NE]->SetContainer(math::AABB::FromCenterAndSize(new_center, new_size));

		// Create South East node from center and size:
		new_center.x = center.x + size.x * 0.25f;
		new_center.z = center.z - size.z * 0.25f;
		children[SE] = new QuadTreeNode();
		children[SE]->SetContainer(math::AABB::FromCenterAndSize(new_center, new_size));

		// Create South West node from center and size:
		new_center.x = center.x - size.x * 0.25f;
		new_center.z = center.z - size.z * 0.25f;
		children[SW] = new QuadTreeNode();
		children[SW]->SetContainer(math::AABB::FromCenterAndSize(new_center, new_size));

		// Create North West node from center and position:
		new_center.x = center.x - size.x * 0.25f;
		new_center.z = center.z + size.z * 0.25f;
		children[NW] = new QuadTreeNode();
		children[NW]->SetContainer(math::AABB::FromCenterAndSize(new_center, new_size));
	}

	const math::AABB& QuadTreeNode::GetContainer() const
	{
		return container;
	}

	void QuadTreeNode::SetContainer(const math::AABB& new_container)
	{
		container = new_container;
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

	void QuadTreeNode::Insert(Entity* const entity)
	{
		const bool is_container_smaller_than_max_box_size =
			(container.HalfSize().LengthSq() <= QUADTREE_MIN_SIZE * QUADTREE_MIN_SIZE);
		const bool has_less_than_maximum_items = entities.size() < QUADTREE_MAX_ITEMS;

		// Store entity on this node if this node is a leaf and it's either smaller than
		// container size or this node has less entities inside than the maximum amount:
		if (IsLeaf() &&
			(has_less_than_maximum_items ||
				is_container_smaller_than_max_box_size))
		{
			entities.push_back(entity);

			return;
		}

		// If this is a leaf and we don't satisfy above conditions, 
		// make create child nodes and make this non-leaf by dividing to 4:
		if (IsLeaf())
		{
			CreateChildren();
		}

		// Add the entity to this node:
		entities.push_back(entity);
		// Distribute the entities between child nodes:
		DistributeChildren();
	}

	void QuadTreeNode::Remove(Entity* const entity)
	{
		// Find the index of the entity inside entities list
		// of the node:
		std::list<Entity*>::iterator index = std::find(entities.begin(), entities.end(), entity);

		// index will be equal to the entities.end() if it's not 
		// found successfully, hence we check for it, if not
		// we erase it from our list:
		if (index != entities.end())
		{
			entities.erase(index);
		}

		// If this is not a leaf node, call this function for
		// the children too:
		if (!IsLeaf())
		{
			for (size_t i = 0; i < 4; ++i)
			{
				children[i]->Remove(entity);
			}
		}
	}

	void QuadTreeNode::DistributeChildren()
	{
		// When called this function will distribute the
		// entities this node has to it's children by
		// calculating an AABB for each entity and checking
		// for intersections with 

		for (std::list<Entity*>::iterator iterator = entities.begin(); iterator != entities.end();)
		{
			// Get the entity:
			Entity* entity = *iterator;

			// Create an aabb that encloses obb of the entity:
			math::AABB bounding_box;
			bounding_box.SetFrom(entity->BoundingBox()->GetBoundingBox());

			// Create an intersections array that will hold if this
			// bounding_box is intersecting with the child node's containers:
			bool intersections[4];
			for (size_t i = 0; i < 4; ++i)
			{
				intersections[i] = bounding_box.Intersects(children[i]->GetContainer());
			}

			// Check if bounding aabb of the entity's obb intersects with all of
			// the children:
			if (intersections[0] && intersections[1] &&
				intersections[2] && intersections[3])
			{
				// If it's intersects with all the children,
				// we can just keep this entity on this node.
				++iterator;
				continue;
			}

			// Erase this entity from this node:
			iterator = entities.erase(iterator);
			// Add it to the children that it intersects with:
			for (size_t i = 0; i < 4; ++i)
			{
				if (!intersections[i])
				{
					continue;
				}

				children[i]->Insert(entity);
			}

		}
	}


	// QuadTree:

	QuadTree::QuadTree() : root_node(nullptr)
	{

	}

	QuadTree::~QuadTree()
	{
		CleanUp();
	}

	QuadTreeNode* const QuadTree::GetRootNode() const
	{
		return root_node;
	}

	void QuadTree::SetContainer(const math::AABB& new_container)
	{
		CleanUp();

		root_node = new QuadTreeNode();

		root_node->SetContainer(new_container);
	}

	void QuadTree::Insert(Entity* const entity)
	{
		if (root_node == nullptr)
		{
			return;
		}

		const math::OBB& entity_obb = entity->BoundingBox()->GetBoundingBox();
		
		if (!entity_obb.MinimalEnclosingAABB().Intersects(root_node->GetContainer()))
		{
			return;
		}

		root_node->Insert(entity);
	}

	void QuadTree::Remove(Entity* const entity)
	{
		if (root_node == nullptr)
		{
			return;
		}
	}

	void QuadTree::CleanUp()
	{
		delete root_node;
	}

}