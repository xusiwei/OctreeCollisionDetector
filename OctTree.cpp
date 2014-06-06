#include "OctTree.h"

// #include <graphics/CDraw3D.h>

#include <stack>
#include <queue>
#include <set>
#include <algorithm>
#include <memory>

#include <float.h>
#include <ctime>
#include <assert.h>

#include "debug.h"

// #define FORCE_QUERY

//#define QUERY_COUNT

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
cVector3d OctreeNode::getNewCenter( int nodeId ) const
{
	cVector3d ret = m_center;
	double w = m_width;

	switch(nodeId) {
	case TOP_LEFT_FRONT:
		ret += cVector3d(-w/4, w/4, w/4); 
		break;
	case TOP_LEFT_BACK:
		ret += cVector3d(-w/4, w/4, -w/4);
		break;
	case TOP_RIGHT_BACK:
		ret += cVector3d( w/4, w/4, -w/4);
		break;
	case TOP_RIGHT_FRONT:
		ret += cVector3d( w/4, w/4, w/4);
		break;
	case BOTTOM_LEFT_FRONT:
		ret += cVector3d( -w/4, -w/4, w/4);
		break;
	case BOTTOM_LEFT_BACK:
		ret += cVector3d( -w/4, -w/4, -w/4);
		break;
	case BOTTOM_RIGHT_BACK:
		ret += cVector3d(  w/4, -w/4, -w/4);
		break;
	case BOTTOM_RIGHT_FRONT:
		ret += cVector3d(  w/4, -w/4,  w/4);
		break;
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool OctreeNode::contains( cVector3d point ) const
{
	cVector3d sub = cSub(m_center, point);
	double wid = m_width;
	if(sub.x < -wid || sub.x > wid) return false;
	if(sub.y < -wid || sub.y > wid) return false;
	if(sub.z < -wid || sub.z > wid) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OctreeNode::draw(cVector3d a_pos) const
{
	cVector3d vertices[8];

	for(int i=0; i<8; ++i) {
		vertices[i] = m_center;
		vertices[i] += a_pos;
	}

	double hw = m_width/2; // half width.
	vertices[TOP_LEFT_FRONT] += cVector3d(-hw, hw, hw);
	vertices[TOP_LEFT_BACK] += cVector3d(-hw, hw, -hw);
	vertices[TOP_RIGHT_BACK] += cVector3d(hw, hw, -hw);
	vertices[TOP_RIGHT_FRONT] += cVector3d(hw, hw, hw);
	vertices[BOTTOM_LEFT_FRONT] += cVector3d(-hw, -hw, hw);
	vertices[BOTTOM_LEFT_BACK] += cVector3d(-hw, -hw, -hw);
	vertices[BOTTOM_RIGHT_BACK] += cVector3d(hw, -hw, -hw);
	vertices[BOTTOM_RIGHT_FRONT] += cVector3d(hw, -hw, hw);

#if 0
	glBegin(GL_QUADS);
	for(int i=0; i<8; ++i)
		glVertex3dv((const GLdouble*)&vertices[i]);
	glEnd();
	glFlush();
	glutSwapBuffers();
#endif

#if 0
	// draw as line frame.
	static int frames[12][2] = {
		// top
		TOP_LEFT_FRONT, TOP_LEFT_BACK,
		TOP_LEFT_BACK, TOP_RIGHT_BACK,
		TOP_RIGHT_BACK, TOP_RIGHT_FRONT,
		TOP_RIGHT_FRONT, TOP_LEFT_FRONT,
		// middle
		TOP_LEFT_FRONT, BOTTOM_LEFT_FRONT,
		TOP_LEFT_BACK, BOTTOM_LEFT_BACK,
		TOP_RIGHT_BACK, BOTTOM_RIGHT_BACK,
		TOP_RIGHT_FRONT, BOTTOM_RIGHT_FRONT,
		// bottom
		BOTTOM_LEFT_FRONT, BOTTOM_LEFT_BACK,
		BOTTOM_LEFT_BACK, BOTTOM_RIGHT_BACK,
		BOTTOM_RIGHT_BACK, BOTTOM_RIGHT_FRONT,
		BOTTOM_RIGHT_FRONT, BOTTOM_LEFT_FRONT
	};

	glBegin(GL_LINES);
	for(int i=0; i<12; ++i)
	{
		glVertex3dv((double*)&vertices[frames[i][0]]);
		glVertex3dv((double*)&vertices[frames[i][1]]);
	}
	glEnd();
	glFlush();
	glutSwapBuffers();
#endif

	cVector3d aCenter = m_center; // actual center.
	aCenter.add(a_pos);

	cDrawWireBox(aCenter.x - m_width/2, aCenter.x + m_width/2,
		aCenter.y - m_width/2, aCenter.y + m_width/2,
		aCenter.z - m_width/2, aCenter.z + m_width/2);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) printf("[OctTreeNode::draw()] Error:  %s\n", gluErrorString(err));
	// printf("[OctTreeNode::draw()] LOG drawing... node_%p\n", this);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OctreeNode::whichVoroniOfCenter(const cVector3d& aRealCenter, const cVector3d& bRealCenter, int& resA, int& resB)
{
	if (aRealCenter.x > bRealCenter.x) // right
	{
		if (aRealCenter.y > bRealCenter.y) // top
		{
			if (aRealCenter.z > bRealCenter.z) // front
			{
				resA = TOP_RIGHT_FRONT;
				resB = BOTTOM_LEFT_BACK;
			} 
			else // back
			{
				resA = TOP_RIGHT_BACK;
				resB = BOTTOM_LEFT_FRONT;
			}
		}
		else // bottom
		{
			if (aRealCenter.z > bRealCenter.z) // front
			{
				resA = BOTTOM_RIGHT_FRONT;
				resB = TOP_LEFT_BACK;
			} 
			else // back
			{
				resA = BOTTOM_RIGHT_BACK;
				resB = TOP_LEFT_FRONT;
			}
		}
	}
	else // left
	{
		if (aRealCenter.y > bRealCenter.y) // top
		{
			if (aRealCenter.z > bRealCenter.z) // front
			{
				resA = TOP_LEFT_FRONT;
				resB = BOTTOM_RIGHT_BACK;
			} 
			else // back
			{
				resA = TOP_LEFT_BACK;
				resB = BOTTOM_RIGHT_FRONT;
			}
		}
		else // bottom
		{
			if (aRealCenter.z > bRealCenter.z) // front
			{
				resA = BOTTOM_LEFT_FRONT;
				resB = TOP_RIGHT_BACK;
			} 
			else // back
			{
				resA = BOTTOM_LEFT_BACK;
				resB = TOP_RIGHT_FRONT;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool OctreeNode::collide( OctreeNode* node ) const
{
	cVector3d thisPos = this->m_tree->getPos();
	cVector3d nodePos = node->m_tree->getPos();

	thisPos.add(this->m_center);
	nodePos.add(node->m_center);

	cVector3d centerLine = cSub(thisPos, nodePos);

	double halfWidSum = (this->m_width + node->m_width) / 2;

	if (fabs(centerLine.x) > halfWidSum) return false;
	if (fabs(centerLine.y) > halfWidSum) return false;
	if (fabs(centerLine.z) > halfWidSum) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct cVector3dComparator
{
	bool operator()(const cVector3d& a, const cVector3d& b) const
	{
#if 0
		if(a.x - b.x > FLT_EPSILON) return false;
		if(a.y - b.y > FLT_EPSILON) return false;
		if(a.z - b.z > FLT_EPSILON) return false;
		return true;
#elif 1
		if(a.x < b.x) return true;
		// if(a.x == b.x) return a.y < b.y;
		if(a.x == b.x && a.y < b.y) return true;
		// if(a.x == b.x && a.y == b.y) return a.z < b.z;
		if(a.x == b.x && a.y == b.y && a.z < b.z) return true;
		return false;
#else
		return a.x < b.x;
#endif
	}
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct HasMeshPointInNode
{
	cMesh* mesh;

	std::vector<cVector3d> vertVec;
	typedef std::vector<cVector3d>::const_iterator VertIter;

	//std::set<cVector3d, cVector3dComparator> vertSet;
	//cVector3dComparator vertComp;

#ifdef QUERY_COUNT
	std::vector<int> xOrder;
	std::vector<int> yOrder;
	std::vector<int> zOrder;

	typedef std::vector<int>::const_iterator VertIdxIter;
	//typedef std::vector<cVector3d>::const_iterator VertIdxIter;
#endif

	HasMeshPointInNode(cMesh* a_mesh) : mesh(a_mesh)
	{
#ifdef QUERY_COUNT
		int numVerts = a_mesh->getNumVertices(true);
		for(int i=0; i<numVerts; ++i)
		{
			// const cVector3d* vt = a_mesh->getVertex(i, true);
			cVertex* pv = a_mesh->getVertex(i, true);
			assert(pv);
			cVector3d pos = pv->getPos();
			vertVec.push_back(pos);

			xOrder.push_back(i);
			yOrder.push_back(i);
			zOrder.push_back(i);
		}

		std::vector<cVector3d>* pVerts = &vertVec;
		std::sort(xOrder.begin(), xOrder.end(), 
			[pVerts](int a, int b){ 
				return (*pVerts)[a].x < (*pVerts)[b].x; 
		});
		std::sort(yOrder.begin(), yOrder.end(), 
			[pVerts](int a, int b){ 
				return (*pVerts)[a].y < (*pVerts)[b].y; 
		});

		std::sort(zOrder.begin(), zOrder.end(), 
			[pVerts](int a, int b){ 
				return (*pVerts)[a].z < (*pVerts)[b].z; 
		});
#else // QUERY_COUNT
		int numVerts = a_mesh->getNumVertices(true);
		for(int i=0; i<numVerts; ++i)
		{
			// const cVector3d* vt = a_mesh->getVertex(i, true);
			cVertex* pv = a_mesh->getVertex(i, true);
			assert(pv);
			{
				cVector3d pos = pv->getPos();
				vertVec.push_back(pos);
			}
		}
#ifndef FORCE_QUERY
		// std::sort(vertVec.begin(), vertVec.end(), cVector3dComparator()); // O(n lg n)
		std::sort(vertVec.begin(), vertVec.end(), 
			[](const cVector3d& va, const cVector3d& vb)
		{
			return va.x < vb.x;
		}
		); // O(n lg n)
#endif // FORCE_QUERY
#endif // QUERY_COUNT
	}

	int operator()(OctreeNode* node) 
	{
#ifdef QUERY_COUNT
		cVector3d vMax(node->m_center);
		cVector3d vMin(node->m_center);
		cVector3d wid3(node->m_width/2, node->m_width/2, node->m_width/2);

		vMin -= wid3;
		vMax += wid3;

		std::vector<cVector3d>* pVerts = &vertVec;
		VertIdxIter xItUpp = std::upper_bound(xOrder.begin(), xOrder.end(),
			vMax, 
			[pVerts](const cVector3d& vec, int idx)
		{
			return vec.x < (*pVerts)[idx].x;
		}
		);

		VertIdxIter xItLow = std::lower_bound(xOrder.begin(), xOrder.end(), 
			vMin, 
			[pVerts](int idx, const cVector3d& vec)
		{
			return (*pVerts)[idx].x < vec.x;
		}
		);

		int xRanged = xItUpp - xItLow;
		if (0 == xRanged) return 0;

		VertIdxIter yItUpp = std::upper_bound(yOrder.begin(), yOrder.end(),
			vMax,
			[pVerts](const cVector3d& vec, int idx)
		{
			return vec.y < (*pVerts)[idx].y;
		}	
		);

		VertIdxIter yItLow = std::lower_bound(yOrder.begin(), yOrder.end(),
			vMin,
			[pVerts](int idx, const cVector3d& vec)
		{
			return (*pVerts)[idx].y < vec.y;
		}	
		);

		int yRanged = yItUpp - yItLow;
		if (0 == yRanged) return 0;

		VertIdxIter zItUpp = std::upper_bound(zOrder.begin(), zOrder.end(), 
			vMax,
			[pVerts](const cVector3d& vec, int idx)
		{
			return vec.z < (*pVerts)[idx].z;
		}	
		);

		VertIdxIter zItLow = std::lower_bound(zOrder.begin(), zOrder.end(),
			vMin,
			[pVerts](int idx, const cVector3d& vec)
		{
			return (*pVerts)[idx].z < vec.z;
		}	
		);

		int zRanged = zItUpp - zItLow;
		if (0 == zRanged) return 0;

		std::set<int> xSet(xItLow, xItUpp);
		std::set<int> ySet(yItLow, yItUpp);
		std::set<int> zSet(zItLow, zItUpp);

		std::vector<int> resXY;
		resXY.resize(xRanged < yRanged ? xRanged : yRanged);

		std::vector<int>::iterator itRes = resXY.begin();
		itRes =	std::set_intersection(xSet.begin(), xSet.end(), ySet.begin(), ySet.end(), resXY.begin());
		int xyRanged = itRes - resXY.begin();

		std::vector<int> resXYZ;
		resXYZ.resize(xyRanged < zRanged ? xyRanged : zRanged );

		itRes = std::set_intersection(resXY.begin(), itRes, zSet.begin(), zSet.end(), resXYZ.begin());

		return int(itRes - resXYZ.begin());
		return 0;
#else // QUERY_COUNT
#ifdef FORCE_QUERY
		for(int i=0, sz=vertVec.size(); i<sz; ++i)
		{
			if (node->contains(vertVec[i]))
			{
				return true;
			}
		}
		return false;
#else // FORCE_QUERY
		cVector3d vMax(node->m_center);
		cVector3d vMin(node->m_center);
		cVector3d wid3(node->m_width/2, node->m_width/2, node->m_width/2);

		vMin -= wid3;
		vMax += wid3;

		VertIter itUpp = std::upper_bound(vertVec.begin(), vertVec.end(), vMax, cVector3dComparator());
		VertIter itLow = std::lower_bound(vertVec.begin(), vertVec.end(), vMin, cVector3dComparator());

		int count = 0;
		if (itLow != itUpp)
		{
			for(VertIter it = itLow; it != itUpp; ++it)
			{
				if (node->contains(*it))
				{
					count++;
					if(count == 2) return 2;
				}
			}
		}

		return 0;
#endif // FORCE_QUERY
#endif // QUERY_COUNT
	}
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Octree::build()
{
	int tstart = std::clock();
	int maxDepth = 0;
	// TODO: construct tree.
	cVector3d bbMin = m_mesh->getBoundaryMin();
	cVector3d bbMax = m_mesh->getBoundaryMax();
	cVector3d bbCenter = m_mesh->getBoundaryCenter();

	cVector3d diagonal = cSub(bbMax, bbMin);
	double dx = diagonal.x;
	double dy = diagonal.y;
	double dz = diagonal.z;

	double dmax = dx;
	if(dy > dmax) dmax = dy;
	if(dz > dmax) dmax = dz;

	m_root = new OctreeNode(NULL);
	m_root->m_center = bbCenter;
	m_root->m_width = dmax;
	m_root->m_hasPoint = true;
	m_root->m_depth  = 0;
	m_root->m_tree = this;

	HasMeshPointInNode hasMeshPiont(m_mesh);

	std::queue<OctreeNode*> nodeQueue;
	nodeQueue.push(m_root);

	// 按层次构造
	while(!nodeQueue.empty()) 
	{
		OctreeNode* parent = nodeQueue.front();
		nodeQueue.pop(); // pop queue.

		assert(parent);
		if(parent->m_width/2 < m_minRadius) break;

		// for debug
		if(parent->m_depth > maxDepth) maxDepth = parent->m_depth + 2;

		OctreeNode* children[OctreeNode::DEGREE];
		for(int i=0; i<OctreeNode::DEGREE; ++i) {
			children[i] = new OctreeNode(parent);
			assert(children[i]);
			children[i]->m_parent = parent;
			children[i]->m_center = parent->getNewCenter(i);
			children[i]->m_width = parent->m_width/2;

			if ( hasMeshPiont(children[i]) )
			{
				nodeQueue.push(children[i]); // push queue.
				children[i]->m_hasPoint = true;
				parent->m_leaf = false;      // 能够展开，不是叶节点
				parent->m_children[i] = children[i];
			}

			children[i]->m_depth = parent->m_depth+1;
			children[i]->m_tree = this;
		}
	}
	int tend = std::clock();
	printf("build tree SUCCESS, depth: %d, time: %d ms.\n", maxDepth+1, tend - tstart);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Octree::render()
{
	if (m_showTree)
	{
		std::queue<OctreeNode*> nodeQue;
		nodeQue.push(m_root);

		cVector3d pos = this->getPos();

		while(!nodeQue.empty())
		{
			OctreeNode* cur = nodeQue.front();
			nodeQue.pop();

			if(cur->m_depth > m_displayDepth) break;
			if(cur->m_depth == m_displayDepth)
				cur->draw(pos);

			for(int i=0; i<OctreeNode::DEGREE; ++i)
			{
				if(cur->m_children[i]) nodeQue.push(cur->m_children[i]);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool Octree::computeCollision( Octree* a_tree, cCollisionRecorder& recoder )
#if 0
{
	std::stack<OctreeNode*> nodeStk;
	std::stack<OctreeNode*> anotherStk;

	nodeStk.push(m_root);
	anotherStk.push(a_tree->m_root);

	while (!nodeStk.empty() && !anotherStk.empty())
	{
		OctreeNode* anode = nodeStk.top();
		nodeStk.pop();

		OctreeNode* bnode = anotherStk.top();
		anotherStk.pop();
		
		cVector3d aTreePos = this->getPos();

		cVector3d bTreePos = a_tree->getPos();

		cVector3d aRealCenter = anode->m_center;
		aRealCenter += aTreePos;

		cVector3d bRealCenter = bnode->m_center;
		bRealCenter += bTreePos;

		double centerDist = aRealCenter.distance(bRealCenter);
		double halfWidSum = (anode->m_width + bnode->m_width) / 2;

		if ( centerDist > halfWidSum)
		{
#if 0
			glBegin(GL_LINE);
				glColor3f(1, 0, 0);
				glVertex3d(anode->m_center.x, anode->m_center.y, anode->m_center.z);
				glVertex3d(bnode->m_center.x, bnode->m_center.y, bnode->m_center.z);
			glEnd();
#elif 0
			PRINT_VECTOR(aRealCenter);
			PRINT_VECTOR(bRealCenter);
			printf("centerDist: %3.4f, ", centerDist);
			printf("halfWidSum: %3.4f\n", halfWidSum);
#endif
			return false;
		}

		// 层次包围树的 下降规则
#if 0
		//////////////////////////////////////////////////////////////////////////
		// ray tracing 下降
		// 向最邻近方向上的节点上扩展
		int vorA, vorB;
		OctreeNode::whichVoroniOfCenter(aRealCenter, bRealCenter, vorA, vorB);

		printf("Voroni A: %d, B:%d\n", vorA, vorB);

		if (anode->m_children[vorA])
		{
			nodeStk.push(anode->m_children[vorA]);
		}

		if (bnode->m_children[vorB])
		{
			anotherStk.push(bnode->m_children[vorB]);
		}
#elif 1
		//////////////////////////////////////////////////////////////////////////
		// 两边同时下降
		// 八分体对枚举下降
		double minDist = DBL_MAX;
		int minA = -1;
		int minB = -1;

		for (int i=0; i<OctreeNode::DEGREE; ++i)
		{
			OctreeNode* subA = anode->m_children[i];

			if (subA)
			{
				cVector3d subAcenter = subA->m_center;
				subAcenter += aTreePos;

				for (int j=0; j<OctreeNode::DEGREE; ++j)
				{
					OctreeNode* subB = bnode->m_children[j];

					if (subB)
					{
						cVector3d subBcenter = subB->m_center;
						subBcenter += bTreePos;

						double dist = subBcenter.distance(subAcenter);
						if (dist < minDist)
						{
							minDist = dist;
							minA = i;
							minB = j;
						}
					}
				}
			}
		}

		printf("min dist sub cube pair: <%d, %d>\n", minA, minB);
		if (minA != -1)
		{
			nodeStk.push(anode->m_children[minA]);
		}
		//else if (anode->m_leaf)
		//{
		//	nodeStk.push(anode);
		//}

		if (minB != -1)
		{
			anotherStk.push(bnode->m_children[minB]);
		}
		//else if (bnode->m_leaf)
		//{
		//	anotherStk.push(bnode);
		//}
#elif 1
		//////////////////////////////////////////////////////////////////////////
		// 单侧下降
		// 下降较大包围体（这种方法较好！）
		double widA = anode->m_width;
		double widB = bnode->m_width;
		
		OctreeNode* pMax = NULL;
		OctreeNode* pMin = NULL;

		cVector3d maxTreePos;
		cVector3d minTreeRealCenter;

		if (widA > widB)
		{
			pMax = anode;
			pMin = bnode;
			maxTreePos = aTreePos;
			minTreeRealCenter = bRealCenter;
		}
		else
		{
			pMax = bnode;
			pMin = anode;
			maxTreePos = bTreePos;
			minTreeRealCenter = aRealCenter;
		}

		for (int i=0; i<OctreeNode::DEGREE; ++i)
		{
			OctreeNode* sub = pMax->m_children[i];
			if (sub)
			{
				cVector3d subCenter = sub->m_center;
				subCenter += maxTreePos;
				if ( subCenter.distance(minTreeRealCenter) > (sub->m_width + pMin->m_width) / 2)
				{
					return false;
				}
			}
		}
#endif
	}

	return true;
}
#else
{
	typedef std::pair<OctreeNode*, OctreeNode*> NodePair;
	std::stack<NodePair> npStack; // node pair stack.
	npStack.push(std::make_pair(this->m_root, a_tree->m_root));

	bool ret = false;
	while ( ! npStack.empty())
	{
		NodePair np = npStack.top();
		npStack.pop();

		if (!np.first->collide(np.second))
		{
			continue;
		}

		if (np.first->m_leaf && np.second->m_leaf)
		{
			// TODO 记录碰撞到 recoder
			recoder.m_nearestCollision.m_localPos = this->getPos(); 
			cCollisionEvent evt;
			evt.m_globalPos = this->getPos();
			recoder.m_collisions.push_back(evt);
			ret = true;
		}
		else 
		{
			// 向下扩展
			// 扩展较大的BVH
			OctreeNode* pMax = np.first;
			OctreeNode* pMin = np.second;

			if (pMin->m_width > pMax->m_width)
				std::swap(pMax, pMin);

			// 按照子方块距离排序
#if 0
			std::multimap<double, OctreeNode*> childMap;
			typedef std::multimap<double, OctreeNode*>::reverse_iterator MapRevIter;
			for (int i=0; i<OctreeNode::DEGREE; ++i)
			{
				OctreeNode* child =  pMax->m_children[i];
				if (child)
				{
					double dist = child->m_center.distance(pMin->m_center);
					childMap[dist] = child;
				}
			}

			printf("map size: %d\n", childMap.size());

			// 以相反顺序压栈
			MapRevIter iter; 
			for (iter = childMap.rbegin(); iter != childMap.rend(); ++iter)
			{
				npStack.push(std::make_pair(iter->second, pMin));
			}
#else
			std::vector<OctreeNode*> vecChilds;
			vecChilds.reserve(OctreeNode::DEGREE);
			for (int i=0; i<OctreeNode::DEGREE; ++i)
			{
				OctreeNode* pChild = pMax->m_children[i];
				if (pChild)
				{
					vecChilds.push_back(pChild);
				}
			}
			// printf("size: %d\n", vecChilds.size());
			std::sort(vecChilds.begin(), vecChilds.end(), 
				[pMin](OctreeNode* pa, OctreeNode* pb) {
					return pa->m_center.distancesq(pMin->m_center) 
						< pb->m_center.distancesq(pMin->m_center);
			});
			
			std::vector<OctreeNode*>::reverse_iterator it;
			for (it = vecChilds.rbegin(); it != vecChilds.rend(); ++it)
			{
				npStack.push(std::make_pair(*it, pMin));
			}
#endif
		}
	}

	return ret;
}
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Octree::destory( OctreeNode* root )
{
	if (root)
	{
		for (int i=0; i<OctreeNode::DEGREE; ++i)
		{
			if (root->m_children[i])
			{
				Octree::destory(root->m_children[i]);
			}
		}
		delete root;
	}
}

bool Octree::validate()
{
	OctreeNode* root = m_root;
	std::stack<OctreeNode*> nodeStack;

	int count = 0;
	nodeStack.push(root);
	while ( ! nodeStack.empty())
	{
		OctreeNode* cur = nodeStack.top();
		nodeStack.pop();
		
		// validate current node
		count++;
		
		printf("node_%p validating...\n", cur);

		bool hasMeshPoint = false;
		if (cur->m_hasPoint)
		{
			int nVert = this->m_mesh->getNumVertices(true);
			for (int i=0; i<nVert; ++i)
			{
				if ( cur->contains(this->m_mesh->getVertex(i, true)->getPos()) )
				{ // has point
					hasMeshPoint = true;
					break;
				}
			}
			if ( ! hasMeshPoint )
			{
				printf("node_%p validate fail!\n", cur);
				printf("node_%p->hasPoint: %s, actual: false\n", cur, cur->m_hasPoint ? "true" : "false");
				return false;
			}
		}

		// for children
		for (int i=0; i<OctreeNode::DEGREE; ++i)
		{
			if (cur->m_children[i])
			{
				nodeStack.push(cur->m_children[i]);
			}
		}
	}
	printf("%d node checked!\n", count);
	return true;
}


