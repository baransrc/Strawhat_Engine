#pragma once

#include "MATH_GEO_LIB/Geometry/AABB.h"
#include "MATH_GEO_LIB/Math/float3.h"

#include <list>
#include <map>

class Entity;
class ComponentMesh;

namespace StrawMath
{
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

		template<typename INTERSECTABLE_T>
		void FillWithIntersections(std::map<float, Entity*>& intersecting_entities, const INTERSECTABLE_T& intersectable) const;
		template<typename INTERSECTABLE_T>
		void FillWithIntersections(std::vector<Entity*>& intersecting_entities, const INTERSECTABLE_T& intersectable) const;

	private:
		QuadTreeNode* parent;
		QuadTreeNode* children[4];
		std::list<Entity*> entities;
		math::AABB container;
	};

	template<typename INTERSECTABLE_T>
	inline void QuadTreeNode::FillWithIntersections(std::map<float, Entity*>& intersecting_entities, const INTERSECTABLE_T& intersectable) const
	{
		// If it's not intersecting with this node's container, halt the method
		// don't even look at the children nodes
		if (!intersectable.Intersects(container))
		{
			return;
		}

		float hit_near = 0.0f;
		float hit_far = 0.0f;

		for (Entity* entity : entities)
		{
			// If this intersects with the current entity's bounding box,
			// insert it to the intersecting_entities by mapping it wrt
			// it's hit_near:
			if (intersectable.Intersects(entity->BoundingBox()->GetBoundingBox(), hit_near, hit_far))
			{
				intersecting_entities[hit_near] = entity;
			}
		}

		// If it's leaf we are done:
		if (IsLeaf())
		{
			return;
		}

		for (size_t i = 0; i < 4; ++i)
		{
			// If the children node has entities with intersections with intersectable
			// as well, call this method on them too:
			children[i]->FillWithIntersections(intersecting_entities, intersectable);
		}
	}

	template<typename INTERSECTABLE_T>
	inline void QuadTreeNode::FillWithIntersections(std::vector<Entity*>& intersecting_entities, const INTERSECTABLE_T& intersectable) const
	{
		if (!intersectable.Intersects(container))
		{
			return;
		}

		for (Entity* entity : entities)
		{
			// If this intersects with the current entity's bounding box,
			// push it back to the intersecting_entities :
			if (intersectable.Intersects(entity->BoundingBox()->GetBoundingBox()))
			{
				intersecting_entities.push_back(entity);
			}
		}

		// If it's leaf we are done:
		if (IsLeaf())
		{
			return;
		}

		for (size_t i = 0; i < 4; ++i)
		{
			// If the children node has entities with intersections with intersectable
			// as well, call this method on them too:
			children[i]->FillWithIntersections(intersecting_entities, intersectable);
		}
	}

	class QuadTree
	{
	private:
		QuadTreeNode* root_node;

	public:
		QuadTree();
		~QuadTree();

		QuadTreeNode* const GetRootNode() const;

		void SetContainer(const math::AABB& new_container);
		void Insert(Entity* const entity);
		void Remove(Entity* const entity);
		void CleanUp();

		template<typename INTERSECTABLE_T>
		void FillWithIntersections(std::map<float, Entity*>& intersecting_entities, const INTERSECTABLE_T& intersectable) const;
		template<typename INTERSECTABLE_T>
		void FillWithIntersections(std::vector<Entity*>& intersecting_entities, const INTERSECTABLE_T& intersectable) const;

	};

	template<typename INTERSECTABLE_T>
	inline void QuadTree::FillWithIntersections(std::map<float, Entity*>& intersecting_entities, const INTERSECTABLE_T& intersectable) const
	{
		if (root_node == nullptr)
		{
			return;
		}

		root_node->FillWithIntersections(intersecting_entities, intersectable);
	}

	template<typename INTERSECTABLE_T>
	inline void QuadTree::FillWithIntersections(std::vector<Entity*>& intersecting_entities, const INTERSECTABLE_T& intersectable) const
	{
		if (root_node == nullptr)
		{
			return;
		}

		root_node->FillWithIntersections(intersecting_entities, intersectable);
	}
}