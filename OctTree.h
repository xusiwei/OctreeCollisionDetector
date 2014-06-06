#ifndef OCT_SPHERE_TREE_H
#define OCT_SPHERE_TREE_H

//#include <math/CVector3d.h>
//#include <graphics/CTriangle.h>
//// #include <collisions/CCollisionBasics.h>
//#include <scenegraph/CGenericObject.h>

#include <chai3d.h>

#include <vector>
#include <cstring>
#include <utility>

enum eOctreeNodes
{
	TOP_LEFT_FRONT,			// 0
	TOP_LEFT_BACK,			// 1
	TOP_RIGHT_BACK,			// etc...
	TOP_RIGHT_FRONT,
	BOTTOM_LEFT_FRONT,
	BOTTOM_LEFT_BACK,
	BOTTOM_RIGHT_BACK,
	BOTTOM_RIGHT_FRONT
};

struct HasMeshPointInNode;

struct CollisionData
{
	cVector3d position;
	std::pair<cVector3d, cVector3d> nearestPair;
};

class Octree;

class OctreeNode
{
	friend class Octree;
	friend struct HasMeshPointInNode;
public:
	static const int DEGREE = 8;

	// ctor and dtor.
	OctreeNode(OctreeNode* a_parent): m_parent(a_parent), m_hasPoint(false), m_leaf(true) { std::memset(m_children, 0, sizeof(m_children)); }

	OctreeNode() : m_center(0,0,0), m_parent(0), m_depth(0), m_hasPoint(false), m_leaf(true) { std::memset(m_children, 0, sizeof(m_children)); }

	~OctreeNode(){}

	// methods:
	inline const cVector3d &getCenter() { return m_center; }

	inline double getRadius() { return m_width; }

	void draw(cVector3d a_pos) const; // finish.

	bool contains(cVector3d point) const; // finish

	bool collide(OctreeNode* node) const;

	static void whichVoroniOfCenter(const cVector3d& aRealCenter, const cVector3d& bRealCenter, int& resA, int& resB);
protected:
	cVector3d getNewCenter(int nodeId) const; // finish

private:
	Octree* m_tree;
	OctreeNode* m_parent;
	OctreeNode* m_children[DEGREE];

	cVector3d m_center;
	double m_width;

	int m_depth;

	bool m_hasPoint;
	bool m_leaf;
	//int m_num;
};

//////////////////////////////////////////////////////////////////////////
// 以顶点云为模型，以小盒进行检测
class Octree  : public cGenericObject
{
public:
	// ctor and dtor:
	Octree(cMesh* a_mesh, double a_minR) : m_showTree(), m_mesh(a_mesh), m_minRadius(a_minR), m_displayDepth(0) {
		m_pos = m_mesh->getPos(); 
		cGenericObject::setPos(m_mesh->getPos());
	}

	cVector3d getPos() { return cGenericObject::getPos(); }

	~Octree(void){ destory(m_root); }

	// methods:
	void render(); // extends from cGenericObject

	void build(); // finish

	bool validate(); 

	bool computeCollision(Octree* a_tree, cCollisionRecorder& recoder);

	void setDisplayDepth(int a_depth) { m_displayDepth = a_depth; }

	void setShowTree(bool a_show) { m_showTree = a_show; }

	bool getShowTree() { return m_showTree; }

protected:
	static void destory(OctreeNode* subtree);

private:
	bool m_showTree;
	OctreeNode* m_root;

	cMesh* m_mesh;
	cVector3d m_pos;
	double m_minRadius;
	int m_displayDepth;
	//	bool m_useNeighbors;
};

#endif //OCT_SPHERE_TREE_H