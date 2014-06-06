#ifndef CONVEX_H
#define CONVEX_H

#include <vector>

// #define USE_CHAI3D

#ifdef USE_CHAI3D
#include <chai3d.h>
#include <glm/glm.hpp>

class Convex
{
public:
	typedef glm::ivec3 iTriangle;

	// ctor and dtor
	Convex(void);
	~Convex(void);

	// 
	void addVertex(const cVertex& vert);

	void addTriangle(const iTriangle& tria);

	void clear();

	// getters:
	cVector3d& getPos() { return m_position; }

	int getNumVertices() { return m_vertices.size(); }

	int getNumTriangles() { return m_triangles.size(); }

	cVertex getVertex(int vIndex) { return m_vertices[vIndex]; }

	void setPos(const cVector3d& pos) { m_position = pos; }

	void setVertex(const cVertex& vert, int vIndex) { m_vertices[vIndex] = vert; }

	// show
	void render();

	// to obj file
	void serialize(const char* filename);
protected:
private:
	cVector3d m_position;

	std::vector<cVertex> m_vertices;
	std::vector<iTriangle> m_triangles;
};

#else // USE_CHAI3D
#include <glm/glm.hpp>

class Convex
{
public:
	typedef float      Float;
	typedef glm::vec3  Point;
	typedef glm::uvec3 Triangle;

	// ctor and dtor
	Convex(void);
	~Convex(void);

	// 
	int addVertex(const Point& vert);

	int addTriangle(const Triangle& tria);

	void clear();

	// getters:
	const Point& getPos() const { return m_position; }

	int getNumVertices() const { return m_numVertices; }

	int getNumTriangles() const { return m_numTriangles; }

	const Triangle& getTriangle(int triIdx) const { return m_triangles[triIdx]; }

	Point getVertex(int vIndex) const { return m_vertices[vIndex]; }

	void setPos(const Point& pos) { m_position = pos; }

	void setVertex(const Point& vert, int vIndex) { m_vertices[vIndex] = vert; }

	// show
	void render();

	// to obj file
	void serialize(const char* filename);
private:
	int m_numVertices;
	int m_numTriangles;

	Point m_position;

	std::vector<Point> m_vertices;
	std::vector<Triangle> m_triangles;
};

#endif // USE_CHAI3D

#endif // CONVEX_H