#include "RegularConvexGenerator.h"


RegularConvexGenerator::RegularConvexGenerator()
{
	m_pConvex = 0;
}

Convex* RegularConvexGenerator::generate( int depth,  float radius )
{
	m_pConvex = new Convex();
	assert(m_pConvex);
	
	m_radius = radius;
	m_culled.clear();
	init();

	if (depth)
	{
		subdivide(0, depth);
		subdivide(1, depth);
		subdivide(2, depth);
		subdivide(3, depth);
		cullTriangles();
	}
	return m_pConvex;
}


typedef Convex::Float    Float;
typedef Convex::Point    Point;
typedef Convex::Triangle iTriangle;

void RegularConvexGenerator::subdivide( int triangle, int depth )
{
	if (0 == depth) return;
	this->m_culled[triangle] = true;
	iTriangle tri = m_pConvex->getTriangle(triangle);

	Point PA = m_pConvex->getVertex(tri[0]);
	Point PB = m_pConvex->getVertex(tri[1]);
	Point PC = m_pConvex->getVertex(tri[2]);

	Point PD = glm::normalize( (PA + PB) / 2.0f );
	Point PE = glm::normalize( (PB + PC) / 2.0f );
	Point PF = glm::normalize( (PA + PC) / 2.0f );

	int iD = m_pConvex->addVertex(PD);
	int iE = m_pConvex->addVertex(PE);
	int iF = m_pConvex->addVertex(PF);

	int tADF = m_pConvex->addTriangle(iTriangle(tri[0], iD, iF));
	m_culled.push_back(false);
	int tBED = m_pConvex->addTriangle(iTriangle(tri[1], iE, iD));
	m_culled.push_back(false);
	int tCFE = m_pConvex->addTriangle(iTriangle(tri[2], iF, iE));
	m_culled.push_back(false);
	int tDEF = m_pConvex->addTriangle(iTriangle(iD, iE, iF));
	m_culled.push_back(false);

	--depth;
	subdivide(tADF, depth);
	subdivide(tBED, depth);
	subdivide(tCFE, depth);
	subdivide(tDEF, depth);
}

static Float vertices[][3] = {
#if 0
	-4.30159e-008, 1, -4.30159e-008,
	0.910684, -0.333333, -0.244017,
	-0.244017, -0.333333, 0.910684,
	-0.666667, -0.333333, -0.666667
#else
	-4.30159e-008f, 1.0f, -4.30159e-008f,
	0.910684f, -0.333333f, -0.244017f,
	-0.244017f, -0.333333f, 0.910684f,
	-0.666667f, -0.333333f, -0.666667f
#endif
};

#define NVERTICES sizeof(vertices)/sizeof(vertices[0])

static int triangles[][3] = {
	0, 2, 1,
	0, 3, 2,
	0, 1, 3,
	1, 2, 3
};

#define NTRIANGLES sizeof(triangles)/sizeof(triangles[0])

void RegularConvexGenerator::init()
{
	assert(m_pConvex);
	for (int i=0; i<NVERTICES; ++i)
	{
		m_pConvex->addVertex(Point(vertices[i][0], vertices[i][1], vertices[i][2]) * m_radius);
	}
	for (int i=0; i<NTRIANGLES; ++i)
	{
		m_pConvex->addTriangle(iTriangle(triangles[i][0], triangles[i][1], triangles[i][2]));
		m_culled.push_back(false);
	}
}

void RegularConvexGenerator::cullTriangles()
{
	Convex* pCulled = new Convex();
	int vCount = 0;

	int nTriangles = m_culled.size();
	for (int i=0; i<nTriangles; ++i)
	{
		if (!m_culled[i])
		{
			iTriangle triangle = m_pConvex->getTriangle(i);
			iTriangle newIndexed;
			for (int j=0; j<3; ++j)
			{
				int vIndex = triangle[j];
				Point vPos = m_pConvex->getVertex(vIndex);
				newIndexed[j] = pCulled->addVertex(vPos);
			}
			pCulled->addTriangle(newIndexed);
		}
	}
	delete m_pConvex;
	m_pConvex = pCulled;
}
