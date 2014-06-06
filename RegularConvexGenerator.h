#ifndef REGULAR_CONVEX_GENERATOR
#define REGULAR_CONVEX_GENERATOR

#include "Convex.h"

class RegularConvexGenerator
{
public:
	RegularConvexGenerator();

	Convex* generate(int depth, float radius = 1.0f);

protected:
	// for generate
	void subdivide(int triangle, int depth);

	void cullTriangles();

	void init();

private:
	float m_radius;
	Convex* m_pConvex;
	std::vector<bool> m_culled;
};

#endif