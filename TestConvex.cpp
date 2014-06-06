#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdio.h>

#include "common.h"
#include "GlutApp.h"
#include "Convex.h"


#ifdef TEST_CONVEX

static Convex* pConvex;

static double angle = 0.0;

static float vertices [][3] = {
	0, 0, 0, // 0
	0, 0, 1, // 1
	0, 1, 0, // 2
	0, 1, 1, // 3

	1, 0, 0, // 4
	1, 0, 1, // 5
	1, 1, 0, // 6
	1, 1, 1  // 7
};

static int triangles[][3] = {
	// left
	0, 1, 3, 
	3, 2, 0,

	// right
	4, 6, 7, 
	7, 5, 4,

	// front
	1, 5, 7,
	7, 3, 1,

	// back
	0, 2, 6,
	6, 4, 0,

	// top
	2, 3, 7,
	7, 6, 2,

	// bottom
	0, 4, 5,
	5, 1, 0
};

class ConvexTest : public GlutApp
{
	void onInit() 
	{
		printf("OnInit\n");

		glClearColor(0.0, 0.0, 0.0, 0.0);

		glEnable(GL_DEPTH_TEST);

		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_LINE);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity(); // 不能少
		gluLookAt(
			3, 4, 5,
			0, 0, 0,
			0, 1, 0
			);
		// glFrustum(-2.0, 2.0, -2.0, 2.0, -1.0, 20.0);
	}

	void onDisplay() 
	{
		// glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glRotated(angle, 1, 1, 1);
		pConvex->render();
	}

	void onResize(int w, int h)
	{
		glViewport(0, 0, w, h);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-2.0, 2.0, -2.0, 2.0);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity(); // 不能少
	}

	void onTimer()
	{
		static int count = 0;
		printf("Alarm %d!\n", count++);

		glutPostRedisplay();
		
		angle += 1.0;
	}
};

int main(int argc, char **argv)
{
	pConvex = new Convex();

#ifdef USE_CHAI3D
	for (int i=0; i<sizeof(vertices)/sizeof(vertices[0]); ++i)
	{
		cVertex vert;
		vert.setPos(vertices[i][0], vertices[i][1], vertices[i][2]);
		vert.setColor(vertices[i][0], vertices[i][1], vertices[i][2]);
		pConvex->addVertex(vert);
	}

	for (int i=0; i<sizeof triangles/sizeof(triangles[0]); ++i)
	{
		Convex::iTriangle tria(triangles[i][0], triangles[i][1], triangles[i][2]);
		pConvex->addTriangle(tria);
	}
#else // USE_CHAI3D
	for (int i=0; i<sizeof(vertices)/sizeof(vertices[0]); ++i)
	{
		pConvex->addVertex(glm::vec3(vertices[i][0], vertices[i][1], vertices[i][2]));
	}

	for (int i=0; i<sizeof triangles/sizeof(triangles[0]); ++i)
	{
		pConvex->addTriangle(glm::ivec3(triangles[i][0], triangles[i][1], triangles[i][2]));
	}
#endif // USE_CHAI3D

#if 0
	pConvex->addVertex(glm::vec3(0, 0, 0)); // 0
	pConvex->addVertex(glm::vec3(0, 0, 1)); // 1
	pConvex->addVertex(glm::vec3(0, 1, 0)); // 2
	pConvex->addVertex(glm::vec3(0, 1, 1)); // 3

	pConvex->addVertex(glm::vec3(1, 0, 0)); // 4
	pConvex->addVertex(glm::vec3(1, 0, 1)); // 5
	pConvex->addVertex(glm::vec3(1, 1, 0)); // 6
	pConvex->addVertex(glm::vec3(1, 1, 1)); // 7

	// left
	pConvex->addTriangle(glm::ivec3(0, 1, 3));
	pConvex->addTriangle(glm::ivec3(3, 2, 0));

	// right
	pConvex->addTriangle(glm::ivec3(4, 6, 7));
	pConvex->addTriangle(glm::ivec3(7, 5, 4));

	// front
	pConvex->addTriangle(glm::ivec3(1, 5, 7));
	pConvex->addTriangle(glm::ivec3(7, 3, 1));

	// back
	pConvex->addTriangle(glm::ivec3(0, 2, 6));
	pConvex->addTriangle(glm::ivec3(6, 4, 0));

	// top
	pConvex->addTriangle(glm::ivec3(2, 3, 7));
	pConvex->addTriangle(glm::ivec3(7, 6, 2));

	// bottom
	pConvex->addTriangle(glm::ivec3(0, 4, 5));
	pConvex->addTriangle(glm::ivec3(5, 1, 0));
#endif


	printf("Number vertices: %d\n", pConvex->getNumVertices());
	printf("Number triangles: %d\n", pConvex->getNumTriangles());

	pConvex->serialize("convex.obj");

	GlutApp* test = new ConvexTest();

	test->initGlut(argc, argv);
	test->setWindowsSize(640, 480);
	test->setTitle("Convex test");

	 test->setTimer(1000, 100);
	//test->setTimer(1000);

	test->run();

	return 0;
}

#endif // APP_TEST