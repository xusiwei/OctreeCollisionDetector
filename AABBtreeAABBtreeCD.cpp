#include "AABBtreeAABBtreeCD.h"

#include "debug.h"

#include <stack>

inline bool intersect(const cCollisionAABBBox& a_0, const cCollisionAABBBox& a_1)
{
	// check for overlap along each axis
	if (a_0.getLowerX() > a_1.getUpperX()) return false;
	if (a_0.getLowerY() > a_1.getUpperY()) return false;
	if (a_0.getLowerZ() > a_1.getUpperZ()) return false;
	if (a_1.getLowerX() > a_0.getUpperX()) return false;
	if (a_1.getLowerY() > a_0.getUpperY()) return false;
	if (a_1.getLowerZ() > a_0.getUpperZ()) return false;

	// if the boxes are not separated along any axis, a collision has occurred
	return true;
}

cCollisionAABB* getAvailableCdFromMesh(cMesh* mesh)
{
	std::stack<cMesh*> meshStack;
	meshStack.push(mesh);
	while (!meshStack.empty())
	{
		cMesh* curMesh = meshStack.top();
		meshStack.pop();

		cCollisionAABB* aabbCD = dynamic_cast<cCollisionAABB*>(curMesh->getCollisionDetector());
		if (aabbCD && aabbCD->getRoot())
		{
			return aabbCD;
		}

		for (int i=0; i<curMesh->getNumChildren(); ++i)
		{
			cMesh* child = dynamic_cast<cMesh*>(curMesh->getChild(i));
			if (child)
			{
				meshStack.push(child);
			}
		}
	}
	return NULL;
}

bool contains(const cCollisionAABBBox &box, const cVector3d& a_p)
{
	if (a_p.x >= box.m_min.x && a_p.y >= box.m_min.y && a_p.z >= box.m_min.z &&
		a_p.x <= box.m_max.x && a_p.y <= box.m_max.y && a_p.z <= box.m_max.z)
		return (true);
	else
		return false;
}

void checkTreeBvh(cCollisionAABBNode* tree)
{
	std::stack<cCollisionAABBNode*> ndStack;
	ndStack.push(tree);

	while (!ndStack.empty())
	{
		cCollisionAABBNode* curNode = ndStack.top();
		ndStack.pop();

		cCollisionAABBInternal* inNode = dynamic_cast<cCollisionAABBInternal*>(curNode);
		if (inNode)
		{
			cCollisionAABBNode* left = inNode->m_leftSubTree;
			cCollisionAABBNode* right = inNode->m_rightSubTree;

			if (left)
			{
				ndStack.push(left);
			}

			if (right)
			{
				ndStack.push(right);
			}
		}
		else
		{
			cCollisionAABBLeaf* leaf = dynamic_cast<cCollisionAABBLeaf*>(curNode);
			if ( leaf && 
				( !contains(tree->m_bbox, curNode->m_bbox.m_max)
				||!contains(tree->m_bbox, curNode->m_bbox.m_min)
				)
			)
			{
				printf("BVH logic error!\n");
				printf("node_%p can not contains node_%p(%d)\n", tree, curNode, curNode->m_depth);
			}

		}
	}
}

AABBtreeAABBtreeCD::AABBtreeAABBtreeCD(void)
{
}

AABBtreeAABBtreeCD::AABBtreeAABBtreeCD( cMesh* meshA, cMesh *meshB )
{
	this->m_meshA = meshA;
	this->m_meshB = meshB;
	this->m_collsionDetectorA = getAvailableCdFromMesh(meshA);
	this->m_collsionDetectorB = getAvailableCdFromMesh(meshB);
#if 0
	checkTreeBvh(this->m_collsionDetectorA->getRoot());
	printf("==============\n");
	checkTreeBvh(this->m_collsionDetectorB->getRoot());
#endif
}

AABBtreeAABBtreeCD::~AABBtreeAABBtreeCD(void)
{
}


bool AABBtreeAABBtreeCD::computeCollision(cCollisionRecorder& a_recorder, cCollisionSettings& a_setting)
{	
	typedef std::pair<cCollisionAABBNode*, cCollisionAABBNode*> NodePair;
	std::stack<NodePair> npStack;
	npStack.push(std::make_pair(
		m_collsionDetectorA->getRoot(),
		m_collsionDetectorB->getRoot()
	));

	bool collid = false;
	while (!npStack.empty())
	{
		cCollisionAABBInternal *ipA = NULL, *ipB = NULL;

		NodePair np = npStack.top();
		npStack.pop();

		cCollisionAABBNode *a, *b;
		a = np.first;
		b = np.second;

		ipA = dynamic_cast<cCollisionAABBInternal*>(a);
		ipB = dynamic_cast<cCollisionAABBInternal*>(b);

#if 0
		cCollisionAABBLeaf* la = dynamic_cast<cCollisionAABBLeaf*>(a);
		cCollisionAABBLeaf* lb = dynamic_cast<cCollisionAABBLeaf*>(b);

		if (la && lb)
		{
			printf("check %d, %d\n", la->m_triangle->m_index, lb->m_triangle->m_index);
		}
#endif

#if 1
		cVector3d aMin, aMax;
		// aMin = a->m_bbox.m_min + m_meshA->getPos();
		a->m_bbox.m_min.addr(m_meshA->getPos(), aMin);
		a->m_bbox.m_max.addr(m_meshA->getPos(), aMax);
		cCollisionAABBBox aBox(aMin, aMax);

		cVector3d bMin, bMax;
		b->m_bbox.m_min.addr(m_meshB->getPos(), bMin);
		b->m_bbox.m_max.addr(m_meshB->getPos(), bMax);
		cCollisionAABBBox bBox(bMin, bMax);

		if (! intersect(aBox, bBox))
		{
			continue;
		}
#else
		cVector3d aPos = m_meshA->getPos();
		cVector3d bPos = m_meshB->getPos();

		cVector3d delta;
		bPos.subr(aPos, delta);

		cVector3d dMin, dMax;
		b->m_bbox.m_min.addr(delta, dMin);
		a->m_bbox.m_max.addr(delta, dMax);

		cCollisionAABBBox dBox(dMin, dMax);

		//cVector3d pos = m_meshB->getPos();
		if ( ! intersect(a->m_bbox, dBox) )
		{
			continue;
		}
#endif
		//PRINT_VECTOR(aMin);
		//PRINT_VECTOR(aMax);
		//PRINT_VECTOR(bMin);
		//PRINT_VECTOR(bMax);

		if (ipA == NULL && ipB == NULL)
		{
			cCollisionAABBLeaf* leafA = dynamic_cast<cCollisionAABBLeaf*>(a);
			cCollisionAABBLeaf* leafB = dynamic_cast<cCollisionAABBLeaf*>(b);
			assert(leafA);
			assert(leafB);
			
			//puts("===================================================================================");
			//PRINT_VECTOR(m_meshA->getPos());
			//PRINT_VECTOR(m_meshB->getPos());

			cVector3d AB, BA;

			// AB = B.meshPos - A.meshPos
			m_meshB->getPos().subr(m_meshA->getPos(), AB);
			AB.negater(BA);

			if (LeafCollide(leafA, leafB, AB, a_recorder, a_setting))
			{
				collid = true;
			}

			if (LeafCollide(leafB, leafA, BA, a_recorder, a_setting))
			{
				collid = true;
			}
			// return a->computeCollision(dMin, dMax, dBox, a_recorder, a_setting);
			// return a->computeCollision(bMin, bMax, bBox, a_recorder, a_setting);
		}
		else
		{
			if (DescendA(a, b))
			{
				assert(ipA);
				npStack.push(std::make_pair(ipA->m_rightSubTree, b));
				npStack.push(std::make_pair(ipA->m_leftSubTree, b));
			}
			else
			{
				assert(ipB);
				npStack.push(std::make_pair(a, ipB->m_rightSubTree));
				npStack.push(std::make_pair(a, ipB->m_leftSubTree));
			}
		}
	}
	return collid;
}

bool AABBtreeAABBtreeCD::NodeCollide( cCollisionAABBNode* a, cCollisionAABBNode* b )
{
#if 0
	cVector3d aMin, aMax;
	// aMin = a->m_bbox.m_min + m_meshA->getPos();
	a->m_bbox.m_min.addr(m_meshA->getPos(), aMin);
	a->m_bbox.m_max.addr(m_meshA->getPos(), aMax);
	cCollisionAABBBox aBox(aMin, aMax);

	cVector3d bMin, bMax;
	b->m_bbox.m_min.addr(m_meshB->getPos(), bMin);
	b->m_bbox.m_max.addr(m_meshB->getPos(), bMax);
	cCollisionAABBBox bBox(bMin, bMax);

	return intersect(aBox, bBox);
#else
	// unresolved
	cVector3d aPos = m_meshA->getPos();
	cVector3d bPos = m_meshB->getPos();

	cVector3d delta;
	bPos.subr(aPos, delta);

	cVector3d dMin, dMax;
	b->m_bbox.m_min.addr(delta, dMin);
	a->m_bbox.m_max.addr(delta, dMax);

	cCollisionAABBBox dBox(dMin, dMax);

	return intersect(a->m_bbox, dBox);
#endif
}

bool AABBtreeAABBtreeCD::DescendA( cCollisionAABBNode* nodeA, cCollisionAABBNode* nodeB )
{
	return IsLeaf(nodeB);
	// return IsLeaf(nodeB) || (!IsLeaf(nodeA) && nodeA->m_bbox.m_extent.lengthsq() >= nodeB->m_bbox.m_extent.lengthsq());
}

bool AABBtreeAABBtreeCD::IsLeaf( cCollisionAABBNode* node )
{
	assert(node);
	cCollisionAABBLeaf* inNode = NULL;
	inNode = dynamic_cast<cCollisionAABBLeaf*>(node);
	return NULL != inNode;
}

bool AABBtreeAABBtreeCD::LeafCollide( cCollisionAABBLeaf* nodeA, cCollisionAABBLeaf* nodeB, const cVector3d& deltaPos, cCollisionRecorder& a_recorder, cCollisionSettings& a_setting)
{
	cTriangle* triA = nodeA->m_triangle;
	cTriangle* triB = nodeB->m_triangle;
	
	assert(triA);
	assert(triB);
	
	cVertex* pB1 = triB->getVertex0();
	cVertex* pB2 = triB->getVertex1();
	cVertex* pB3 = triB->getVertex2();

	assert(pB1);
	assert(pB2);
	assert(pB3);

	cVector3d vBA0, vBA1, vBA2;

	deltaPos.addr(pB1->getPos(), vBA0); // vBA1 = baVec + pB1->getPos();
	deltaPos.addr(pB2->getPos(), vBA1);
	deltaPos.addr(pB3->getPos(), vBA2);

#if 0
	PRINT_BOX(&nodeA->m_bbox);

	PRINT_VECTOR(triA->getVertex0()->getPos());
	PRINT_VECTOR(triA->getVertex1()->getPos());
	PRINT_VECTOR(triA->getVertex2()->getPos());

	PRINT_VECTOR(vBA0);
	PRINT_VECTOR(vBA1);
	PRINT_VECTOR(vBA2);

	PRINT("%d\n", intersect(nodeA->m_bbox, nodeB->m_bbox));
	PRINT("%d\n", nodeA->m_bbox.contains(vBA0));
	PRINT("%d\n", nodeA->m_bbox.contains(vBA1));
	PRINT("%d\n", nodeA->m_bbox.contains(vBA2));
#else
	//printf("check %d, %d\n", nodeA->m_triangle->m_index, nodeB->m_triangle->m_index);
#endif

	if ( ! nodeA->m_bbox.contains(vBA0)
	  && ! nodeA->m_bbox.contains(vBA1)
	  && ! nodeA->m_bbox.contains(vBA2))
	{
		return false;
	}

	bool res = false;
	
	if (triA->computeCollision(vBA0, vBA1, a_recorder, a_setting))
	{
		res = true;
	}

	if (triA->computeCollision(vBA0, vBA2, a_recorder, a_setting))
	{
		res = true;
	}

	if (triA->computeCollision(vBA1, vBA2, a_recorder, a_setting))
	{
		res = true;
	}

	return res;
}
