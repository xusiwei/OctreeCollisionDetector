#ifndef AABBTREE_AABBTREE_CD_H
#define AABBTREE_AABBTREE_CD_H

#include <chai3d.h>

class AABBtreeAABBtreeCD
{
public:
	AABBtreeAABBtreeCD(void);
	
	// AABBtreeAABBtreeCD(cCollisionAABB* cdA, cCollisionAABB *cdB);

	AABBtreeAABBtreeCD(cMesh* meshA, cMesh *meshB);
	
	~AABBtreeAABBtreeCD(void);

	bool computeCollision(cCollisionRecorder& a_recorder , cCollisionSettings& a_setting);

protected:
	bool NodeCollide(cCollisionAABBNode* a, cCollisionAABBNode* b);

	static bool DescendA(cCollisionAABBNode* nodeA, cCollisionAABBNode* nodeB);

	static bool IsLeaf(cCollisionAABBNode* node);

	bool LeafCollide(cCollisionAABBLeaf* nodeA, cCollisionAABBLeaf* nodeB, const cVector3d& deltaPos, cCollisionRecorder& a_recorder, cCollisionSettings& a_setting);

private:
	cMesh* m_meshA;
	cMesh* m_meshB;

	cCollisionAABB* m_collsionDetectorA;
	cCollisionAABB* m_collsionDetectorB;
};

#endif // AABBTREE_AABBTREE_CD_H